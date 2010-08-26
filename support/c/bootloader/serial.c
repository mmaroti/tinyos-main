/*****************************************************************************
*
* Atmel Corporation
*
* File              : serial.c
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
* Description       : UART communication routines
****************************************************************************/
#include "defines.h"

void initbootuart(void)
{
  BAUD_RATE_LOW_REG = BRREG_VALUE;
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
                     (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit 
// UCSR0C = (0 << USBS0) | (3 << UCSZ00);
}


void sendchar(unsigned char c)
{
  UART_DATA_REG = c;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}


unsigned char recchar(void)
{
  counter=100000;
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
    return UART_DATA_REG;
  }
}
