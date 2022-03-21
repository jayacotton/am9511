export CPMDrive_D = ./
export CPMDefault = d:
CC = zcc
LINK = zcc
CFLAGS = +cpm -create-app --list --c-code-in-asm -lmath48 
LFLAGS = +cpm -create-app -lmath48

all: iotest.hex test1

iotest.hex: iotest.mac
	macro iotest
	asm8080 iotest.asm -l
	java -jar VirtualCpm.jar d:load d:iotest.hex
#	sudo cp iotest.com /var/www/html/IOTEST.COM

test1: dec2flt.c
	$(CC) $(CFLAGS) dec2flt.c -otest1
#	$(LINK) $(LFLAGS) -otest1 dec2flt.o

test: 
	mv test1 test1.com
	java -jar VirtualCpm.jar d:test1

install:
	cp *.COM /cygdrive/c/xampp/htdocs/.

clean:
	rm iotest.hex iotest.asm iotest.list iotest.lst iotest.bin
	rm test1 TEST1.COM

