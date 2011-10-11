configuration HplBma180C {
  provides interface FastSpiByte;
  provides interface AsyncStdControl;
  provides interface GeneralIO as CSN;
  provides interface GeneralIO as SCK;
}
implementation {
  components  UsartSpiC as SpiC, AtmegaGeneralIOC as IO;
  
  AsyncStdControl = SpiC;
  FastSpiByte = SpiC;
  CSN = IO.PortB6;
  SCK = IO.PortE2;
}
