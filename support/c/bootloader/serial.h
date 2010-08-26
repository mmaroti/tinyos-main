/*****************************************************************************
*
* Atmel Corporation
*
* File              : serial.h
* Compiler          : IAR C 3.10C Kickstart, AVR-GCC/avr-libc(>= 1.2.5)
* Revision          : $Revision: 1.1 $
* Date              : $Date: 2010-08-26 21:03:24 $
* Updated by        : $Author: szabomeister $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description       : Header file for serial.c
****************************************************************************/

void initbootuart( void );
void sendchar( unsigned char );
unsigned char recchar( void );
