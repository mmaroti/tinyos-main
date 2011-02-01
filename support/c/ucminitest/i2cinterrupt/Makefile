all: i2c

i2c: i2c.elf
	avr-objcopy -O ihex i2c.elf i2c.hex
i2c.elf: i2c.o
	avr-gcc -g -mmcu=atmega128rfa1 -o i2c.elf i2c.o 
i2c.o: i2c.c
	avr-gcc -g -Os -mmcu=atmega128rfa1 -c i2c.c	
	
prog: i2c.hex
	avrdude -cavr109 -C/etc/avrdude/avrdude.conf -pm128rfa1 -P/dev/ttyUSB0 -Uflash:w:i2c.hex:i -b115200
dump: i2c.elf
	avr-objdump -h -S i2c.elf > i2c.lst
	
clean:
	rm i2c.o 
	rm i2c.elf 
	rm i2c.hex


#	

#	

	
