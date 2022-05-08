This repo is to support S100computers Math_board v1.0

dec2flt.c is a "C" program that converts Interegers to floating point
format.   The limiting factor for this program is ATOL, it can't deal with
floating point format.

Testing this code I have found 2 things of interest.  1.  The program will
not/never run on a z180 chip.  And 2. There is a slight rounding error between
this c program and the basic code.  The C code is more accurate in my limited
test.

flt2dec.bas and dec2flt are microsoft basic
programs to convert a decimal value to hex and
back to decimal.

Used to creat the constant hex value for PI ..  40490fdb


I am provideing iotest.asm in case you need to modify the i/o addresses or change
the way the program works.
