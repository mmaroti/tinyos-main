configuration SpiImpC {
  provides interface Init;
  provides interface SpiByte;
  provides interface FastSpiByte;
  provides interface SpiPacket;
  provides interface Resource[uint8_t id];
}
implementation {
  components SpiImpM, HplImpC;
  components new SimpleFcfsArbiterC("Atm128SpiC.Resource") as Arbiter;
  components McuSleepC;

  Init        = SpiImpM;
  SpiByte     = SpiImpM;
  FastSpiByte = SpiImpM;
  SpiPacket   = SpiImpM;
  Resource    = SpiImpM;

  SpiImpM.ArbiterInfo -> Arbiter;
  SpiImpM.ResArb -> Arbiter;
  SpiImpM.Spi -> HplImpC;
  SpiImpM.McuPowerState -> McuSleepC;

  components DiagMsgC;
  SpiImpM.DiagMsg -> DiagMsgC;
}
