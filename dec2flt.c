/* the example code for decimal to float convertion of a floating
point number for the am9511 was coded in basic.... moving it to
C so I can convert it to z80 assembly from there */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* subroutine will convert a ascii text string of digits to a
32 bit floating point number in binary 
Currently we can only support whole numbers, no decimal points */

unsigned char a[40];
unsigned long res;
long d;
int i;
double e;
double m;
double j;

unsigned long
dec2flt (char *string)
{

  memset (a, 0, 32);
  if (strlen (string) == 0)
    return (0);
  d = atol (string);
  if (d == 0)
    return 0;
  if (d <= 0)
    {
      d -= d;			/* change the sign */
      a[31] = 1;
    }
  modf (log ((double)d) / log (2.0),&e);

  m = (float) d / pow (2.0, e);
  e += 127.0;
  j = 256.0;
  for (i = 30; i >= 23; i--)
    {
      j = j / 2.0;
      if (e >= j)
	{
	  a[i] = 1;
	  e = e - j;
	}
    }
  m -= 1;
  j = 1;
  for (i = 22; i >= 0; i--)
    {
      j = j / 2;
      if (m >= j)
	{
	  a[i] = 1;
	  m -= j;
	}
    }
/* assemble the 32 bit hex result.  a(n) has the bits in reverse
order */
  res = 0;
  for (i = 31; i >= 0; i -= 4)
    {
      res = res << 4;
      res |= 8 * a[i] + 4 * a[i - 1] + 2 * a[i - 2] + a[i - 3];
    }
  return (res);
}

void
main ()
{
  printf ("0  ->%lx\n", dec2flt ("0"));
  printf ("1  ->%lx\n", dec2flt ("1"));
  printf ("2  ->%lx\n", dec2flt ("2"));
  printf ("3  ->%lx\n", dec2flt ("3"));
  printf ("4  ->%lx\n", dec2flt ("4"));
  printf ("5  ->%lx\n", dec2flt ("5"));
  printf ("6  ->%lx\n", dec2flt ("6"));
  printf ("7  ->%lx\n", dec2flt ("7"));
  printf ("8  ->%lx\n", dec2flt ("8"));
  printf ("9  ->%lx\n", dec2flt ("9"));
  printf ("10 ->%lx\n", dec2flt ("10"));

}
