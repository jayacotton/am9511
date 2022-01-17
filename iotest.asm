;
;  This code will test if it is possiblel to write a number to the FPU and read it
;  back.
;
;  It has a scope loop to allow repeated writes or reads.
;
;  It has code to cause the FPU to push PI onto the stack and read out
;  the floating point value, to be compaired to an in core copy.
;
;  You may need to modify this code to work with your version of the assembler.
;  I am useing my asm8080 program.
;
;  This code is written in calmac32 and assembled with asm8080 (see my repo)
;  it will run on 8080/z80 and 8085.
;
;#include "../macro-library/pre.mac"
;#include "../macro-library/post.mac"
;#include "../macro-library/prestack.mac"
;#include "../macro-library/wto.mac"
;#include "../macro-library/cpmcommon.mac"


	include	"defs.inc"
	include "io.inc"
Switch	equ	255
; possible switch settings for selecting the test.
S1	equ	1	; write loop
S2	equ	2	; read loop
S3	equ	4	; write/read loop
S4	equ	8	; write command and read result
S5	equ	16	; go back to cpm

Statled equ	255
;	common
bdos    equ     5h      ; dos entry point
bios    equ     5h      ; dos entry point
reset   equ     0       ; jump here to reset
consi   equ     1       ; read console
conso   equ     2       ; type function
getcos	equ	11	; get console status
getver	equ	12	; get cp/m version
seldisk equ     14      ; select disk
openf   equ     15      ; file open
closef  equ     16      ; close file
srchf   equ     17      ; get first directory entry
srchn   equ     18      ; search next directory entry
delf    equ     19      ; delete file
readf   equ     20      ; read sequencial
writef  equ     21      ; write sequencial
creatf  equ     22      ; creat file
renf    equ     23      ; rename file
getdriv	equ	24	; get a list of log-in drives....
getcur	equ	25	; get the current driver number
setdma	equ	26	; set disk i/o buffer address
readran equ     33      ; read random record
writran equ     34      ; write random record
cfs	equ	35      ; compute file size
cfgtbl	equ	69	; return the config table
;
fcb	equ	5ch	; location of file control block
buff	equ	80h	; location of disk input buffer
DEFAULT	equ	buff
;
cr	equ	0dh
lf	equ	0ah
typef	equ	conso
printf	equ	9	
;
; file control block stuff
;
fcbdn	equ	fcb+0	; disk name
fcbfn	equ	fcb+1	; file name
fcbft	equ	fcb+9	; file type
fcbrl	equ	fcb+12	; current reel number
fcbrc	equ	fcb+15	; current record number mod 128
fcbcr	equ	fcb+32	; next record number
fcbln	equ	fcb+33	; fcb length
	org	100h	; placed here to make it convenient for cpm.
;	pre
	lxi	h,0
	dad	sp
	shld	oldsp
	lxi	sp,newsp

start	call	init
sloop	in	Switch
	cpi	S1
	jz	test1
	cpi	S2
	jz	test2
	cpi	S3
	jz	test3
	cpi	S4
	jz	test4
	cpi	S5
	jz	done	; back to cpm
	jmp	sloop
;
;  write to am9511 scoop loop
;
test1	equ	$ 
	in	Switch	; make sure the user still wants this test
	cpi	S1
	jnz	done
	lxi	h,PI
	call	AMwrt
	jmp	test1
; 
;  read from am9511 scoop loop
;
test2	equ	$
	in	Switch
	cpi	S2
	jnz	done
	lxi	h,Buff
	call	AMred
	jmp	test2
;
;  write/read am9511 scoop loop
;
test3	equ	$
	in	Switch
	cpi	S3
	jnz	done
	lxi	h,PI
	call	AMwrt
	lxi	h,Buff
	call	AMred
	jmp	test3
;
;  write commands to am9511 and read back results.
;
;  this is a simple test, force the FPU to put PI on the stack
;  and read it back to see if it does it.
;
test4	equ	$
	in	Switch
	cpi	S4
	jnz	done
	mvi	a,PUPI	; push PI to TOS in the FPU
	call	AMWrcmd
	lxi	h,Buff
	call	AMred	; Read back the value, should be PI
; check the results
	lxi	h,Buff
	lxi	d,PI	; Pre calculated value of PI
	mvi	c,4
t4l	ldax	d
	cmp	m
	jnz	Bad
	inx	h
	inx	d
	dcr	c
	jnz	t4l
	jmp	test4
;Bad	wto	'Compair error'
Bad	equ	$
	push	h
	push	d
	push	b
	jmp	a168169
b168169	db	'Compair error'
	db	cr,lf
	db	'$'
a168169	equ	$
	lxi	d,b168169
	call	Wto
	pop	b
	pop	d
	pop	h
	jmp	x168169
Wto	equ	$
	mvi	c,printf
	call	bdos	
	ret
x168169	equ	$
	jmp	test4
;
;  init the program and any am9511 stuff needed.
;
init	equ	$
;	wto	'IoTest for AM9511'
	push	h
	push	d
	push	b
	jmp	a171463
b171463	db	'IoTest for AM9511'
	db	cr,lf
	db	'$'
a171463	equ	$
	lxi	d,b171463
	call	Wto
	pop	b
	pop	d
	pop	h
	ret
;
; i/o support code for test program
;
;   AMwrt on entry hl -> a floating point number
;    a and c get mangled
;   clobbers de
;
AMwrt	equ	$
	call	AMWait
	lxi	d,3
	dad	d
	mvi	c,4
AMw1	mov	a,m
	dcx	h
	out	APUDr
	dcr	c
	jnz	AMw1
	ret
;
;  AMred on entry hl -> output buffer for floating point number
;   a and c get mangled
;  clobbers de
;
AMred	equ	$
	call	AMWait
	lxi	d,3
	dad	d
	mvi	c,4
AMr1	in	APUDr
	mov	m,a
	dcx	h
	dcr	c
	jnz	AMr1
	ret
;
;  AMWait.  Wait here for the am9511 to be NOT busy
;   a get clobbered
;
AMWait	equ	$
	in	APUSr
	ani	Busy
	jnz	AMWait
	ret
;
; AMWrcmd on entry a has a command byte
;
AMWrcmd	equ	$
	push	psw
	call	AMWait
	pop	psw
	out	APUCr
	ret
PI	db	40h,49h,0fh,0dbh
Buff	ds	6
;
; all done go to cpm
;
done	equ	$
;	post
	lhld	oldsp
	sphl
	ret
;	prestack
oldsp   dw      0
        ds      128
newsp   equ     $
	end
