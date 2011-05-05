configuration HplUsartSpiC {
  provides interface UsartSpi;
}
implementation {
  components HplUsartSpiP, HplBma180C, McuSleepC;

  UsartSpi = HplUsartSpiP;

  HplUsartSpiP.SS               -> HplBma180C.CSN;
  HplUsartSpiP.SCK              -> HplBma180C.SCK;
  HplUsartSpiP.McuPowerOverride <- McuSleepC;
  HplUsartSpiP.Mcu              -> McuSleepC;
}
