generic module HplUartC(uint8_t uartnum, uint32_t baudrate){
  provides interface StdControl;
  provides interface UartByte;
}
implementation{

  command error_t StdControl.start()
  {
    uint32_t BRREG_VALUE = (uint32_t)MHZ*1000000UL / ((uint32_t)16*baudrate) - 1;
    if(uartnum==0){
      UBRR0L = BRREG_VALUE;
      UBRR0H= (BRREG_VALUE << 8);
      UCSR0A = 0;//(1<<U2X0);
      UCSR0B = (1 << RXEN0) | (1 << TXEN0); // enable receive and transmit
      UCSR0C = (0 << USBS0) | (3 << UCSZ00); // setting uart frame format 8 data bits; 1 stop bit
    } else {
      UBRR1L = BRREG_VALUE;
      UBRR1H= (BRREG_VALUE << 8);
      UCSR1A = 0;//(1<<U2X1);
      UCSR1B = (1 << RXEN1) | (1 << TXEN1); // enable receive and transmit
      UCSR1C = (0 << USBS1) | (3 << UCSZ10); // setting uart frame format 8 data bits; 1 stop bit
    }
    return SUCCESS;
  }
  
  command error_t StdControl.stop()
  {
    UCSR1B = 0;
    return SUCCESS;
  }
  
  async command error_t UartByte.send( uint8_t byte ){
    if(uartnum==0){
      UDR0 = byte;                                   // prepare transmission
      while (!(UCSR0A & (1 << TXC0)));// wait until byte sendt
      UCSR0A |= (1 << TXC0);          // delete TXCflag
    } else {
      UDR1 = byte;                                   // prepare transmission
      while (!(UCSR1A & (1 << TXC1)));// wait until byte sendt
      UCSR1A |= (1 << TXC1);          // delete TXCflag
    }
    return SUCCESS;
  }
  
  async command error_t UartByte.receive( uint8_t* byte, uint8_t timeout ){
    //TODO
    #warning UART receive is not implemented
    return SUCCESS;
  }

}