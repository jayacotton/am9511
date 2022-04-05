export CPMDrive_D = ./
export CPMDefault = d:
CC = zcc
RM = rm -f
LINK = zcc
#CFLAGS = +cpm -create-app --list --c-code-in-asm -lmath32
#CFLAGS = +cpm -create-app --list --c-code-in-asm --am9511
#CFLAGS = +cpm -create-app --list --c-code-in-asm -lmath48 -m
#CFLAGS =  +rc2014 -subtype=cpm -create-app --am9511 
CFLAGS =  +rc2014 -subtype=cpm -create-app -lmath32

all: iotest.hex dec2flt mathtest

iotest.hex: iotest.mac
	macro iotest
	asm8080 iotest.asm -l
	java -jar VirtualCpm.jar d:load d:iotest.hex

dec2flt: dec2flt.c
	$(CC) $(CFLAGS) dec2flt.c -odec2flt

mathtest: mathtest.c font.c snaplib.c liboled.c
#mathtest: mathtest.o font.o snaplib.o liboled.o
#	$(CC) $(CFLAGS) mathtest.o font.o liboled.o snaplib.o -omathtest
	$(CC) $(CFLAGS) mathtest.c font.c snaplib.c liboled.c -omathtest
#	$(CC) $(CFLAGS) mathtest.c font.o snaplib.o -omathtest

mathtest.o:
	$(CC) $(CFLAGS) -c mathtest.c

snaplib.o:
	$(CC) $(CFLAGS) -c snaplib.c

font.o:
	$(CC) $(CFLAGS) -c font.c

liboled.o:
	$(CC) $(CFLAGS) -c liboled.c

test: 
	mv dec2flt.bin dec2flt.com

install:
#	cp dec2flt.com /cygdrive/c/xampp/htdocs/DEC2FLT.COM
	cp MATHTEST.COM /cygdrive/c/xampp/htdocs/.

clean:
	$(RM) iotest.hex iotest.asm iotest.list iotest.lst iotest.bin
	$(RM) dec2flt DEC2FLT.COM *.list *.lis *.bin
	$(RM) mathtest *.lst *.COM
	$(RM) *.o

