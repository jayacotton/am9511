;  FPU opcodes 
;   Note:  AR bit.   I have opcodes for AR = 0  If are is to be 1
;   then OR the AR bit onto the opcode.

AR	equ	80h	; see note above

; TOS = Top Of Stack
; NOS = Next On Stack
;  I will boldly claim that the stack is 4 32 bit words
;  or 8 16 bit words deep.

;    16-bit fixed point operations

SADD	equ	6ch	; Add TOS to NOS.  Result to NOT.  Pop Stack.
SSUB	equ	6dh	; Subtract TOS from NOT. Result to NOS, Pop Stack.
SMUL	equ	6eh	; Multiply NOS ty TOS. Lower result to NOS. Pop Stack.
SMUU	equ	76h	; Multiply NOT by TOS, Upper result to NOS. POP Stack.
SDIV	equ	6FH	; Divide NOS by TOS, Result to NOS Pop Stack.

;   32 - bit fixed point operations

DADD	equ	2ch	; Add TOS to NOS.  Result to NOS. Pop Stack
DSUB	equ	2dh	; Subtract TOS from NOS. Result to NOS. Pop Stack
DMUL	equ	2eh	; Multiply NOS by TOS.  Lower result to NOS. Pop Stack
DMUU	equ	36h	; Multiply NOS by TOS.  Upper result to NOS. Pop Stack
DDIV	equ	2fh	; Divide NOS by TOS.  Result to NOS. Pop Stack.

;  32 bit floating point primary operations

FADD	equ	10h	; Add TOS to NOS. Result to NOS. Pop Stack
FSUB	equ	11h	; Subtrack TOS from NOS. Result to NOS. Pop Stack.
FMUL	equ	12h	; Multiply NOS by TOS. Result to NOS. Pop Stack.
FDIV	equ	13h	; Divide NOS by TOS. Result to NOS. Pop Stack.

;  32 bit floating point derived operations

SQRT	equ	01h	; Square Root of TOS.  Result to TOS.
SIN	equ	2h	; Sine of TOS. Result to TOS.
COS	equ	3h	; Cosine of TOS.  Result to TOS.
TAN	equ	4h	; Tangent of TOS.  Result to TOS.
ASIN	equ	5h	; Inverse Sine of TOS.  Result to TOS.
ACOS	equ	6h	; Inverse Cosine of TOS.  Result to TOS.
ATAN	equ	7h	; Inverse Tangent of TOS.  Result to TOS.
LOG	equ	8h	; Common Log of TOS Result to TOS.
LN	equ	9h	; Natural Log of TOS Result to TOS.
EXP	equ	0ah	; e raised to power in TOS.  Result to TOS.
PWR	equ	0bh	; NOS raised to power in TOS, Result to NOS. Pop Stack

;  data and stack manipulation operations

NOP	equ	0h	; No Op.  Ceare or Set SVREQ
FIXS	equ	1fh	; Convert TOS from Float to Fixed 
FIXD	equ	1eh	;  "
FLTS	equ	1dh	; Convert TOS from fixed to Float
FLTD	equ	1ch	;  "
CHSS	equ	74h	; Change sign of fixed point in TOS
CHSD	equ	34h	;  "
CHSF	equ	15h	; Change sign of float on TOS
PTOS	equ	77h	; Push stack.  Make duplicate NOS from TOS
PTOD	equ	37h	;  "
PTOF	equ	17h	;  "
POPS	equ	78h	; Pop stack. Old NOS to TOS, old TOS to bottom 
POPD	equ	38h	;  "	
POPF	equ	18h	;  "
XCHS	equ	79h	; Exchange TOS and NOS
XCHD	equ	39h	;  "
XCHF	equ	19h	;  "
PUPI	equ	1Ah	; Push float 'PI' to TOS, Previous TOS to NOS

; Status register bits.

Busy	equ	80h	; 1 = chip is busy working.
TOSNeg	equ	40h	; 1 = Top of stack is Neg.
TOSZero	equ	20h	; 1 = TOS is zero.
EMask	equ	18h
ArgSize	equ	18h	; 11 = argument to large.
DivZero	equ	10h	; 10 = divide by zero.
NegArg	equ	8h	; 01 = Negative Arg.
Under	equ	4h	; 1 = Underflow
Over	equ	2h	; 1 = Overflow
Carry	equ	1h	; 1 = Carry or Borrow


