configuration HplImpC {
  provides interface Atm128Spi as SpiBus;
}
implementation {
  components HplAtm128GeneralIOC as IO, HplImpP;

  SpiBus = HplImpP;

  HplImpP.SS   -> IO.PortB6;
  HplImpP.SCK  -> IO.PortE2;
  HplImpP.MOSI -> IO.PortE1;
  HplImpP.MISO -> IO.PortE0;
}
