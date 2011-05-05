configuration UsartSpiC {
  provides interface FastSpiByte;
  provides interface AsyncStdControl;
}
implementation {
  components UsartSpiP as MasterSpi, HplUsartSpiC as HplSpi;
  
  AsyncStdControl = MasterSpi;
  FastSpiByte     = MasterSpi;
  
  MasterSpi.Spi -> HplSpi.UsartSpi;

  components LedsC;
  MasterSpi.Leds -> LedsC;

  components DiagMsgC;
  MasterSpi.DiagMsg -> DiagMsgC;
}
