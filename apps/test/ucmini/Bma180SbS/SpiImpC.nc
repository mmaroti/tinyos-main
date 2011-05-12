configuration SpiImpC {
  provides interface SpiByte;
  provides interface FastSpiByte;
  provides interface Resource[uint8_t id];
}
implementation {
  components SpiImpM, HplImpC;
  components new SimpleFcfsArbiterC("SbS.Resource") as Arbiter;


  SpiByte     = SpiImpM;
  FastSpiByte = SpiImpM;
  Resource    = SpiImpM;

  SpiImpM.ArbiterInfo -> Arbiter;
  SpiImpM.ResArb -> Arbiter;
  SpiImpM.Spi -> HplImpC;

  components DiagMsgC;
  SpiImpM.DiagMsg -> DiagMsgC;
}
