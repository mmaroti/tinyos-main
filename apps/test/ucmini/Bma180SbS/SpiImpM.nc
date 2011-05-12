module SpiImpM {
  provides interface SpiByte;
  provides interface FastSpiByte;
  provides interface Resource[uint8_t id];

  uses interface Atm128Spi as Spi;
  uses interface Resource as ResArb[uint8_t id];
  uses interface ArbiterInfo;

  uses interface DiagMsg;
}
implementation {

  void startSpi()
	{
		/*UBRR0 = 0;
		DDRE |= (1 << PE2);
		UCSR0C = (1 << UMSEL01) | (1 << UMSEL00) | (0 << UCPHA0) | (0 << UCPOL0);
		UCSR0B = (1 << RXEN0) | (1 << TXEN0);
		UBRR0 = ((PLATFORM_MHZ*1000000) / (2* 0xFFF))-1;//100;	// baudrate*/
    call Spi.enableSpi(FALSE);
    atomic {
      //call Spi.setClock(0);
      UBRR0 = 0;
      call Spi.initMaster();
      call Spi.enableInterrupt(FALSE);
      call Spi.setMasterDoubleSpeed(TRUE);  
      call Spi.setClockPolarity(TRUE);
      call Spi.setClockPhase(TRUE);      
      call Spi.enableSpi(TRUE);
      call Spi.setClock(8);
    }
	}

  async command uint8_t SpiByte.write( uint8_t data ) {
    uint8_t rcvd=0xFF;
		//while ( !( UCSR0A & (1<<UDRE0)) )
			;

		//UDR0 = data;
    call Spi.write(data);

		while ( !(UCSR0A & (1<<RXC0)) )
			;
    //rcvd = UDR0;
    rcvd = call Spi.read();
    if(call DiagMsg.record()) {
      call DiagMsg.str("write");
      call DiagMsg.hex8(data);
      call DiagMsg.hex8(rcvd);
      call DiagMsg.send();
    }
		return rcvd;
  }

  inline async command void FastSpiByte.splitWrite(uint8_t data) {
    call Spi.write(data);
  }

  inline async command uint8_t FastSpiByte.splitRead() {
    while( !( call Spi.isTransferDone() ) )
      ;
    return call Spi.read();
  }

  inline async command uint8_t FastSpiByte.splitReadWrite(uint8_t data) {
    uint8_t b;

    while( !( call Spi.isTransferDone() ) )
	;
    b = call Spi.read();
    call Spi.write(data);

    return b;
  }

  inline async command uint8_t FastSpiByte.write(uint8_t data) {
    call Spi.write(data);
    while( !( call Spi.isTransferDone() ) )
      ;
    return call Spi.read();
  }

  async command error_t Resource.request[uint8_t id]() {
    atomic{
      if(!call ArbiterInfo.inUse()) {
        startSpi();
      }
    }
    
    return call ResArb.request[ id ]();
  }

  async command error_t Resource.immediateRequest[uint8_t id]() {
    error_t result = call ResArb.immediateRequest[ id ]();
   if ( result == SUCCESS ) {
     startSpi();
   }
   return result;
  }

  async command bool Resource.isOwner[uint8_t id]() {
     return call ResArb.isOwner[id]();
  }

  async command error_t Resource.release[uint8_t id]() {
    error_t error = call ResArb.release[ id ]();
   atomic {
     if (!call ArbiterInfo.inUse()) {
       //stopSpi();
     }
   }
   return error;
  }

  event void ResArb.granted[ uint8_t id ]() {
   signal Resource.granted[ id ]();
 }

  async event void Spi.dataReady(uint8_t data) {
  }

   default event void Resource.granted[ uint8_t id ]() {}
}
