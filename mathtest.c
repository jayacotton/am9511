/* the example code for decimal to float convertion of a floating
point number for the am9511 was coded in basic.... moving it to
C so I can convert it to z80 assembly from there */

/* default configuration is
decimal mode
rc2014 platform
oled display
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include "font.h"
#include "oled.h"

extern int snapmem (long *, long *, int, int, char *);
extern unsigned char image[MAXBUF];

#ifdef __SDCC
#define ASM __asm
#define ENDASM __asmend;
#else
#define ASM #asm
#define ENDASM #endasm
#endif

ASM
        GLOBAL init_floatpack
init_floatpack: ret
ENDASM

#define MATH_DATA_PORT	0x42
#define MATH_CTRL_PORT	0x43
#define MATH_STATUS_PORT 0x43

#define CHIP_8255_A	0x45
#define CHIP_8255_B	0x46
#define CHIP_8255_C	0x47
#define CHIP_8255_CTRL_PORT	0x48
#define SADD	0x06C
#define SSUB	0x06D
#define SMUL	0x06E
#define SMUU	0x0E6
#define DADD	0x02C
#define DMUL	0x02E
#define FLTD	0x01C
#define FIXD	0x01E
#define SQRT	0x001
#define FADD	0x10
#define FSUB	0x11
#define FMUL	0x12
#define FDIV	0x13
#define FPUSH	0x17
#define FPOP	0x1b

unsigned long FirstNumber;
unsigned long SecondNumber;
unsigned long ResultNumber;

/*debug stuff*/
unsigned int hl;
unsigned int de;
unsigned int bc;
void DumpReg();

/* subroutine will convert a ascii text string of digits to a
32 bit floating point number in binary 
Currently we can only support whole numbers, no decimal points */

void
usage ()
{
  printf ("(R)   Reset the AM9511 Chip\n");
  printf ("(+)   Add two 16 bit numbers.      (XXXXH + YYYYH=ZZZZH)\n");
  printf ("(-)   Subtract two 16 bit numbers. (XXXXH - YYYYH=ZZZZH)\n");
  printf ("(M)   Multiply 16 X 8 bit bit number. (XXXXH X YYH=ZZZZH)\n");
  printf
    ("(D)   Add two 32 bit numbers.      (XXXXXXXXH + YYYYYYYYH=ZZZZZZZZH)\n");
  printf
    ("(X)   Multiply 32 X 16 bit bit number. (XXXXXXXXH X YYYYH=ZZZZZZZZH)\n");
  printf ("(C)   Convert 32 bit number to floating format\n");
  printf ("(E)   Convert floating format to 32 bit number\n");
  printf ("(S)   Get the square root of a floating point number\n");
  printf ("(ESC) Quit\n");
}

void
prompt ()
{
  printf ("Please enter command > ");
}

void
signon ()
{
  printf ("9511 Math Board Test Program.  V0.1A2 (John Monahan 2/4/2022)\n");
  printf ("Please note all values need to be 16 bit HEX numbers.\n");
}


void
reset_9511 ()
{
/* *INDENT-OFF* */ 
ASM 
    	IN A, (MATH_DATA_PORT)
    	IN A, (MATH_CTRL_PORT)
	RET 
ENDASM 
/* *INDENT-ON* */
}

float fl;
unsigned long lfl;
/* char *c is a pointer to a decimal string */
/* convert to IEEE-754 float and then to */
/* am9511-float long */
/* BUGS:  the base program is running in math48 mode, so the floats are
48 bits long */

unsigned long
dec2flt (char *c)
{
  sscanf (c, "%f", &fl);	/* IEEE-754 */
  printf ("\ndec2flt %s %f %lx ", c, fl, fl);
/* *INDENT-OFF* */
ASM
/* convert fl to am9511-float format */
	ld	hl,(_fl)
	ld	de,(_fl+2)
	ld	(_hl),hl
	ld	(_de),de
	ld	(_bc),bc
	push	hl
	push	de
	push	bc
	push	psw
	call	_DumpReg  
	pop	psw
	pop	bc
	pop	de
	pop	hl
; from z88dk am32_pushf.asm
	ld	a,d   ; capture exponent
    	sla 	e     ; position exponent in a
    	rl 	a     ; check for zero
    	jr 	Z,_zero
    	cp 	127+63; check for overflow
    	jr 	NC,_max
    	cp 	127-64; check for underflow
    	jr 	C,_zero
    	sub 	127-1 ; bias including shift 
		      ;binary point
    	rla           ; position 7-bit exponent 
		      ; for sign
    	rl 	d     ; get sign
    	rra
    	ld 	d,a   ; restore exponent
    	scf           ; set mantissa leading 1
    	rr 	e     ; restore mantissa
; store de hl as finished product
._done	
	ld	(_hl),hl
	ld	(_de),de
	ld	(_bc),bc
	push	hl
	push	de
	push	bc
	push	psw
	call	_DumpReg  
	pop	psw
	pop	bc
	pop	de
	pop	hl
	ld	(_lfl),hl
	ld	(_lfl+2),de
ENDASM

/* *INDENT-ON* */

  return (lfl);
/* *INDENT-OFF* */
ASM
._zero
    ld de,0                     ; no signed zero available
    ld h,d
    ld l,e
    jp _done 

._max        ; floating max value of sign d in dehl
    ld a,d
    and 080h                    ; isolate sign
    or 03fh                     ; max exponent
    ld d,a

    ld e, 0ffh                  ; max mantissa
    ld h,e
    ld l,e
    jp _done 
ENDASM
/* *INDENT-ON* */
}

/* unsigned long n is the am9511-float format as a 32 bit number */
/* exit with a pointer to a decimal string representation of an */
/* IEEE-754 float */
unsigned char *
flt2dec (unsigned long n)
{
  char text[80];
  lfl = n;
  printf ("flt2dec %lx ", lfl);
  /* convert am9511-float to IEEE-754 */
/* *INDENT-OFF* */
ASM
	ld	de,(_lfl+2)
	ld	hl,(_lfl)
	in	a,(MATH_STATUS_PORT)
; from z88dk am32_popf.asm
	and 07ch                    ; errors from status register
    jp NZ,errors

    sla e                       ; remove leading 1 from mantissa

    ld a,d                      ; capture exponent
    rla                         ; adjust twos complement exponent
    sra a                       ; with sign extention
    add 127-1                   ; bias including shift binary point

    rl d                        ; get sign
    rra                         ; position sign and exponent
    rr e                        ; resposition exponent and mantissa
    ld d,a                      ; restore exponent
	jp	_done1

.errors
    rrca                        ; relocate status bits (just for convenience)
    bit 5,a                     ; zero
    jp NZ,_zero
    bit 1,a
    jp NZ,infinity              ; overflow
    bit 2,a
    jp NZ,_zero                  ; underflow

.nan
    rl d                        ; get sign
    ld de,0feffh
    rr d                        ; nan exponent
    ld h,e                      ; nan mantissa
    ld l,e
	jp	_done1
.infinity
    rl d                        ; get sign
    ld de,0fe80h
    rr d                        ; nan exponent
    ld hl,0                     ; nan mantissa
._done1
	ld	(_fl+2),hl
	ld	(_fl+4),de
ENDASM
/* *INDENT-ON* */
  /* convert IEEE-754 to decimal string */
  sprintf (text, "%f", fl);
printf("\n");
snapmem(&fl,&fl,6,0x24,"fl ");
  printf ("%s %f %lx\n", text, fl, fl);
  return (text);
}

GetNumber (unsigned long *result)
{
  char text[80];		/* space for big string */
  scanf ("%s\n", text);
  *result = dec2flt (text);
}

void
pioinit ()
{
}

/* output data to the am9511.
* store digits in the hex buffer
*/
long temp;
void
DataOut (long v)
{
  temp = v;
/* *INDENT-OFF* */
ASM
	ld	hl,(_temp)
	ld	de,(_temp+2)
	ld	(_hl),hl
	ld	(_de),de
	ld	bc,MATH_DATA_PORT
	ld	(_bc),bc
	out	(c),l
	out	(c),h
	out	(c),e
	out	(c),d
ENDASM
/* *INDENT-ON* */
}

/* input data from the am9511
*/
void
DataIn (unsigned long *p)
{
  while (inp (MATH_STATUS_PORT) & 0x80);
/* *INDENT-OFF* */
ASM
	ld	bc,MATH_DATA_PORT
	ld	(_bc),bc
	in	d,(c)
	in	e,(c)
	in	h,(c)
	in	l,(c)
	ld	(_temp+2),de
	ld	(_temp),hl
	ld	(_hl),hl
	ld	(_de),de
ENDASM
/* *INDENT-ON* */
  *p = temp;
}

/* write a command to the am9511
*/
unsigned char vx;
void
CmdOut (v)
{
  vx = v;
/* *INDENT-OFF* */
ASM
	ld	a,(_vx);
	out	(MATH_CTRL_PORT),a
ENDASM
/* *INDENT-ON* */
}

/* clear the chip status
*/
void
ClearStatus ()
{
}

int
WaitCmdComplete ()
{
  while (inp (MATH_STATUS_PORT) & 0x80);
  return 0;
}

void
Display (int l, float v)
{
  unsigned char text[80];
  int pos;
  unsigned char *p;
  sprintf (text, "%f", v);
/* convert string and display digits */
  pos = 0;
  line (l);
  p = text;

  while (*p)
    {
      if (*p == '.')
	{
	  draw ('.', pos);
	}
      else
	{
	  draw (*p - 0x30, pos);
	}
      p++;
      pos += 1;
    }
}

void
ShowStat ()
{
  unsigned char stat;
  stat = inp (MATH_STATUS_PORT);
  if (stat & 1)
    printf ("Carry or Borrow\n");
  if (stat & 2)
    printf ("Overflow\n");
  if (stat & 4)
    printf ("Underflow\n");
  if (stat & 0x18)
    {
      switch (stat & 0x18)
	{
	case 8:
	  printf ("Negative arg\n");
	  break;
	case 0x10:
	  printf ("Zero divisor \n");
	  break;
	case 0x18:
	  printf ("Arg to large\n");
	  break;
	default:
	  printf ("uh ?\n");
	  break;
	}
    }
  if (stat & 0x20)
    printf ("TOS is zero \n");
  if (stat & 0x40)
    printf ("TOS is negative\n");
  if (stat & 0x80)
    printf ("Busy\n");
}
void DumpReg()
{
	printf("\nHL %04x\n",hl);
	printf("DE %04x\n",de);
	printf("BC %04x\n",bc);
}
void
SIMPLE_ADD ()
{
  reset_9511 ();
printf("%s\n",flt2dec(0x03a00000));
printf("%s\n",flt2dec(0x07f60000));
printf("%s\n",flt2dec(0x7dfbe76c));
printf("%s\n",flt2dec(0x819df3b6));
exit(0);
  printf ("confidence %s\n", flt2dec (dec2flt ("5.0")));
  printf ("%8lx\n", dec2flt ("5."));
  printf ("%8lx\n", dec2flt ("5"));
  printf ("%8lx\n", dec2flt ("123"));
  printf ("%s\n", flt2dec (dec2flt (".123")));
  exit (0);
  FirstNumber = SecondNumber = ResultNumber = 0;
  printf ("\nPlease enter the first decimal number \n");
  GetNumber (&FirstNumber);
  DataOut (FirstNumber);
  ShowStat ();
  DataIn (&FirstNumber);
  ShowStat ();
  CmdOut (FPUSH);
  ShowStat ();
  Display (LINE0, flt2dec (FirstNumber));
  refresh ();
  printf ("\nPlease enter the second decimal number \n");
  GetNumber (&SecondNumber);
  DataOut (SecondNumber);
  DataIn (&SecondNumber);
  Display (LINE0, flt2dec (FirstNumber));
  Display (LINE1, flt2dec (SecondNumber));
  refresh ();
  CmdOut (FADD);
  ShowStat ();
  printf ("\nPlease enter a CR to display the result of the calculation.");
  getchar ();
  if (WaitCmdComplete ())
    return;
  DataIn (&ResultNumber);
  Display (LINE0, flt2dec (FirstNumber));
  Display (LINE1, flt2dec (SecondNumber));
  Display (LINE2, flt2dec (ResultNumber));
  refresh ();
}

void
SIMPLE_SUBTRACT ()
{
}

void
SIMPLE_MULTIPLY_X8 ()
{
}

void
SIMPLE_32_ADD ()
{
}

void
SIMPLE_32_X8 ()
{
}

void
TO_FLOAT_NUMBER ()
{
}

void
FROM_FLOAT_NUMBER ()
{
}

void
SQUARE_FLOAT_NUMBER ()
{
}

void
MULTIPLY_32_X8 ()
{
}

long v;
void
main ()
{
  unsigned char a;
  int j;
  oled_init ();
  oled_clear ();
  oled_set_full_screen ();
  signon ();
  pioinit ();
  reset_9511 ();
  while (1)
    {
      usage ();
      prompt ();
      a = getchar ();
      switch (a)
	{
	case 0x1b:
	  exit (0);
	  break;
	case 'R':
	case 'r':
	  reset_9511 ();
	  break;
	case '+':
	case '=':
	case 'A':
	case 'a':
	  SIMPLE_ADD ();
	  break;
	case '-':
	case '_':
	  SIMPLE_SUBTRACT ();
	  break;
	case 'M':
	case 'm':
	  SIMPLE_MULTIPLY_X8 ();
	  break;
	case 'D':
	case 'd':
	  SIMPLE_32_ADD ();
	  break;
	case 'X':
	case 'x':
	  MULTIPLY_32_X8 ();
	  break;
	case 'C':
	case 'c':
	  TO_FLOAT_NUMBER ();
	  break;
	case 'E':
	case 'e':
	  FROM_FLOAT_NUMBER ();
	  break;
	case 'S':
	case 's':
	  SQUARE_FLOAT_NUMBER ();
	  break;
	case 'H':
	case 'h':
	  break;
	case 'N':
	case 'n':
	  break;
	default:
	  usage ();
	}
    }
}


