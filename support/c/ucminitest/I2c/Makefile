all: main

main: main.elf
	avr-objcopy -O ihex main.elf main.hex
main.elf: main.o
	avr-gcc -g -mmcu=atmega128rfa1 -o main.elf main.o 
main.o: main.c
	avr-gcc -g -Os -mmcu=atmega128rfa1 -c main.c	
	
prog: main.hex
	avrdude -cavr109 -C/etc/avrdude/avrdude.conf -pm128rfa1 -P/dev/ttyUSB0 -Uflash:w:main.hex:i -b115200
dump: main.elf
	avr-objdump -h -S main.elf > main.lst
	
clean:
	rm main.o 
	rm main.elf 
	rm main.hex


#	

#	

	
