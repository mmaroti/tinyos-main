module HplSpiC{
  provides interface SpiByte;
  provides interface StdControl;
}
implementation{
  command error_t StdControl.start(){
    TOSH_MAKE_SPI_MOSI_OUTPUT();
    TOSH_MAKE_SPI_CLK_OUTPUT();
    TOSH_MAKE_SPI_SS_OUTPUT();
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(3<<SPR0);
    return SUCCESS;
  }
  
  command error_t StdControl.stop(){
    SPCR = 0;
    return SUCCESS;
  }
  
  async command uint8_t SpiByte.write( uint8_t tx ){
    SPDR=tx;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
  }
}