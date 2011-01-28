/*****************************************************************************
*
* Atmel Corporation
*
* File              : serial.c
* Compiler          : IAR C 3.10C Kickstart, AVR-GCC/avr-libc(>= 1.2.5)
* Revision          : $Revision: 1.5 $
* Date              : $Date: 2010/11/19 22:43:58 $
* Updated by        : $Author: szabomeister $
*
* Support mail      : avr@atmel.com
*
* Target platform   : All AVRs with bootloader support
*
* AppNote           : AVR109 - Self-programming
*
* Description       : UART communication routines
****************************************************************************/
#include "defines.h"

void initbootuart(void)
{
  BRREG_VALUE = baudrateRegister(BAUD_RATE);

  BAUD_RATE_LOW_REG = BRREG_VALUE;
  BAUD_RATE_HIGH_REG= (BRREG_VALUE << 8);
  UART_STATUS_REG = (1<<UART_DOUBLE_SPEED_BIT);
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
                     (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit
  #if PLATFORM == IRIS
  UCSR0C = (0 << USBS0) | (3 << UCSZ00);
  #else  
  UCSR1C = (0 << USBS1) | (3 << UCSZ10); // setting uart frame format 8 data bits; 1 stop bit
  #endif
}


void sendchar(unsigned char c)
{
  UART_DATA_REG = c;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}


unsigned char recchar(void)
{
  if (blinker){
	ledSet(0);
    #if PLATFORM == IRIS
    led1On();
    #else
	if(isWriting)
		led2On();
	else
		led1On();
    #endif
  }
  counter=SERIAL_TRY;
  while( (!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT))) && (counter>0))
  {
    counter--;
  }// wait for data
  if (counter==0)
  {
    //sendchar('Q');
    return 255;
  }
  else
  {
    #if PLATFORM == IRIS
    led1Off();
    #else
    led0On();
    #endif
    return UART_DATA_REG;
  }
}
