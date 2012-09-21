// $Id: MicaBusP.nc,v 1.4 2006-12-12 18:23:42 vlahan Exp $
/*
 * Copyright (c) 2005-2006 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */
/**
 * Internal component of the simplistic mica bus interface.
 * @author David Gay
 */

module MicaBusP {
  provides {
    interface MicaBusAdc as Adc0;
    interface MicaBusAdc as Adc1;
    interface MicaBusAdc as Adc2;
    interface MicaBusAdc as Adc3;
    interface MicaBusAdc as Adc4;
    interface MicaBusAdc as Adc5;
    interface MicaBusAdc as Adc6;
    interface MicaBusAdc as Adc7;
    interface Init;
  }
  uses{
    interface GeneralIO as PW0;
    interface GeneralIO as PW1;
    interface GeneralIO as PW2;
    interface GeneralIO as PW3;
    interface GeneralIO as PW4;
    interface GeneralIO as PW5;
    interface GeneralIO as PW6;
    interface GeneralIO as PW7;
    interface GeneralIO as Int0;
    interface GeneralIO as Int1;
    interface GeneralIO as Int2;
    interface GeneralIO as Int3;
    interface GeneralIO as USART1_CLK;
    interface GeneralIO as USART1_RXD;
    interface GeneralIO as USART1_TXD;
    interface GeneralIO as UART0_RXD;
    interface GeneralIO as UART0_TXD;
    interface GeneralIO as SPI_SCK;
    interface GeneralIO as SPI_MOSI;
    interface GeneralIO as SPI_MISO;
    interface GeneralIO as I2C_CLK;
    interface GeneralIO as I2C_DATA;
    interface GeneralIO as Adc0_IO;
    interface GeneralIO as Adc1_IO;
    interface GeneralIO as Adc2_IO;
    interface GeneralIO as Adc3_IO;
    interface GeneralIO as Adc4_IO;
    interface GeneralIO as Adc5_IO;
    interface GeneralIO as Adc6_IO;
    interface GeneralIO as Adc7_IO;
  }
}
implementation {
  async command uint8_t Adc0.getChannel() { return 0; }
  async command uint8_t Adc1.getChannel() { return 1; }
  async command uint8_t Adc2.getChannel() { return 2; }
  async command uint8_t Adc3.getChannel() { return 3; }
  async command uint8_t Adc4.getChannel() { return 4; }
  async command uint8_t Adc5.getChannel() { return 5; }
  async command uint8_t Adc6.getChannel() { return 6; }
  async command uint8_t Adc7.getChannel() { return 7; }
  
  command error_t Init.init(){
    //ground all the pins in case they're used as generalIO
    call PW0.makeOutput();
    call PW0.clr();
    call PW1.makeOutput();
    call PW1.clr();
    call PW2.makeOutput();
    call PW2.clr();
    call PW3.makeOutput();
    call PW3.clr();
    call PW4.makeOutput();
    call PW4.clr();
    call PW5.makeOutput();
    call PW5.clr();
    call PW6.makeOutput();
    call PW6.clr();
    call PW7.makeOutput();
    call PW7.clr();
    call Int0.makeOutput();
    call Int0.clr();
    call Int1.makeOutput();
    call Int1.clr();
    call Int2.makeOutput();
    call Int2.clr();
    call Int3.makeOutput();
    call Int3.clr();
    call USART1_CLK.makeOutput();
    call USART1_CLK.clr();
    call USART1_RXD.makeOutput();
    call USART1_RXD.clr();
    call USART1_TXD.makeOutput();
    call USART1_TXD.clr();
    call UART0_RXD.makeOutput();
    call UART0_RXD.clr();
    call UART0_TXD.makeOutput();
    call UART0_TXD.clr();
    call SPI_SCK.makeOutput();
    call SPI_SCK.clr();
    call SPI_MISO.makeOutput();
    call SPI_MISO.clr();
    call SPI_MOSI.makeOutput();
    call SPI_MOSI.clr();
    call I2C_CLK.makeOutput();
    call I2C_CLK.clr();
    call I2C_DATA.makeOutput();
    call I2C_DATA.clr();
    call Adc0_IO.makeOutput();
    call Adc0_IO.clr();
    call Adc1_IO.makeOutput();
    call Adc1_IO.clr();
    call Adc2_IO.makeOutput();
    call Adc2_IO.clr();
    call Adc3_IO.makeOutput();
    call Adc3_IO.clr();
    call Adc4_IO.makeOutput();
    call Adc4_IO.clr();
    call Adc5_IO.makeOutput();
    call Adc5_IO.clr();
    call Adc6_IO.makeOutput();
    call Adc6_IO.clr();
    call Adc7_IO.makeOutput();
    call Adc7_IO.clr();
    //disable jtag
//     MCUCR|=1<<JTD;
//     MCUCR|=1<<JTD;//not a typo, this must be written twice
    return SUCCESS;
  }
}
