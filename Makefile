export CPMDrive_D = ./
export CPMDefault = d:

iotest.hex: iotest.mac
	macro iotest
	asm8080 iotest.asm -l
	vcpm a:load d:iotest.hex
	sudo cp iotest.com /var/www/html/IOTEST.COM

clean:
	rm iotest.hex iotest.asm iotest.list iotest.lst iotest.bin

