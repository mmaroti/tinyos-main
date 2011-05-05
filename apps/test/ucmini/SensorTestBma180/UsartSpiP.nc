module UsartSpiP {
  provides interface FastSpiByte;
  provides interface AsyncStdControl; 
  uses     interface UsartSpi as Spi;
  uses     interface Leds;
  uses     interface DiagMsg;
}
implementation {

  async command error_t AsyncStdControl.start() {
    call Spi.SpiInit(7);
    return SUCCESS;
  }

  async command error_t AsyncStdControl.stop() {
    call Spi.enableSpi(FALSE);
    return SUCCESS;
  }
  
	async command void FastSpiByte.splitWrite(uint8_t data) {
    while ( !( UCSR0A & (1<<UDRE0) ) )
      ; 
    call Spi.write(data);
  }

	async command uint8_t FastSpiByte.splitRead() {
    uint8_t recv;
    while(  !( UCSR0A & (1<<RXC0) )  )
      ;
    recv = call Spi.read();

    return recv; 
  }

	async command uint8_t FastSpiByte.splitReadWrite(uint8_t data) {
    uint8_t recv = call Spi.read();
    while ( !( UCSR0A & (1<<UDRE0) ) )
      ; 
    call Spi.write(data);
  }

	async command uint8_t FastSpiByte.write(uint8_t data) {
    uint8_t recv;
    call Spi.chipSelect();
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");
    asm volatile("nop":::"memory");

    while ( !( UCSR0A & (1<<UDRE0) ) )
      ; 
    call Spi.write(data);

    while(  !( UCSR0A & (1<<RXC0) )  )
      ;
    recv = call Spi.read();
    if(call DiagMsg.record()) {
      call DiagMsg.hex8(recv);
      call DiagMsg.send();
    }
    call Spi.chipDeselect();
    return recv; 
  }

}
