configuration HplImpC {
  provides interface Atm128Spi as SpiBus;
}
implementation {
  components AtmegaGeneralIOC as IO, HplImpP;
  components McuSleepC;

  SpiBus = HplImpP;

  HplImpP.Mcu  -> McuSleepC;
  HplImpP.McuPowerOverride <- McuSleepC;
  HplImpP.SS   -> IO.PortB6;
  HplImpP.SCK  -> IO.PortE2;
  HplImpP.MOSI -> IO.PortE1;
  HplImpP.MISO -> IO.PortE0;
}
