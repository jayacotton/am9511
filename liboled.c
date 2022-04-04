#include <stdio.h>
#include <string.h>
#include "font.h"
extern void oled_init ();
extern void oled_test ();
extern void oled_normal ();
extern void oled_invert ();
extern void oled_clear ();
extern void oled_set_full_screen ();
extern void oled_draw_image (char *, int);
extern void oled_command (char);
extern void oled_data (char);
extern void oled_clean ();

#ifdef __SDCC
#define ASM __asm
#else
#define ASM #asm
#endif

#ifdef __SDCC
#define ENDASM __asmend
#else
#define ENDASM #endasm
#endif

unsigned char image[MAXBUF];
#define CELLSIZE (MAXHEIGHT)
#define CELLSPACE CELLSIZE

int tline;
int loc;
int eloc;

void
line (int v)
{
  tline = v;
}

unsigned char *p;
void
placebits (unsigned char b, int row, int col)
{
  loc = row;
  eloc = col;
  loc += eloc;
  p = image;
  p += loc;
  *p = b;
}

extern char *getbitmap (int);
int offset;
unsigned char *pf;
int width, height;
char curbyte;

void
draw (char c, int o)
{
  int i;
  offset = tline + (6 * o);
/* offset is number of pixels from left side of display */
  pf = (unsigned char *) getbitmap (c);
  for (i = 0; i < 6; i++)
    {
      placebits (pf[i], i, offset);
    }
}

void
refresh ()
{
  oled_set_full_screen ();
  oled_draw_image (image, MAXBUF);
}


char Sequence[] = {
  0xAE, 0xD5, 0xA0, 0xA8,
  0x1F, 0xD3, 0x00, 0xAD,
  0x8E, 0xD8, 0x05, 0xA1,
  0xC8, 0xDA, 0x12, 0x91,
  0x3F, 0x3F, 0x3F, 0x3F,
  0x81, 0x80, 0xD9, 0xD2,
  0xDB, 0x34, 0xA6, 0xA4,
  0xAF, 0xFF
};

char lchar;

void
oled_clearrow ()
{
  int i;
  oled_command (0x10);
  oled_command (4);
  for (i = 0; i < 0x80; i++)
    {
      oled_data (0);
    }

}

unsigned char clearscreen[] = {
  0xb0, 0xb1, 0xb2, 0xb3
};

void
oled_clear ()
{
  int i;
  memset (image, 0, MAXBUF);
  for (i = 0; i < sizeof (clearscreen); i++)
    {
      oled_command (clearscreen[i]);
      oled_clearrow ();
    }
}

unsigned char fullscreen[] = {
  0x20, 0x0, 0x20, 0x4, 0x83,
  0x22, 0x0, 0x3, 0xb0, 0x10,
  0x4
};

void
oled_set_full_screen ()
{
  int i;
  for (i = 0; i < sizeof (fullscreen); i++)
    {
      oled_command (fullscreen[i]);
    }
}

void
oled_draw_image (char *data, int size)
{
  int i;
  i = size;
  do
    {
      oled_data (*data);
      ++data;
      --i;
    }
  while (i);
}

void
oled_data (char c)
{
  lchar = c;
/* *INDENT-OFF* */
ASM
	ld 	c,050h
	ld	b,1
	ld	a,(_lchar)
	out	(c),a
	nop
	nop
	nop
	nop
	set	1,b
	out	(c),a
	nop
	nop
ENDASM
/* *INDENT-ON* */

}

void
oled_test ()
{
  int i;
  oled_command (0xb0);
  oled_command (0x10);
  oled_command (4);
  i = 0;
  while (i <= 0x80)
    {
      oled_data (i++);
    }
}

void
oled_command (char c)
{
  lchar = c;
/* *INDENT-OFF* */
ASM
	ld	c,050h
	ld	b,0
	ld	a,(_lchar)
	out	(c),a
	nop
	nop
	nop
	nop
	set	1,b		;  no idea why
	out	(c),a
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
ENDASM
/* *INDENT-ON* */
}

void
oled_reset ()
{
/* *INDENT-OFF* */
ASM
	ld	c,050h
	ld	b,6
	out	(c),a	; dont understand what is being written 
	nop
	nop
	nop
	nop
	res	2,b
	out	(c),a
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
ENDASM
/* *INDENT-ON* */
}

void
oled_init ()
{
  char *p;
  oled_reset ();
  p = Sequence;
  while (*p != 0xFF)
    {
      oled_command (*p++);
    }
}
