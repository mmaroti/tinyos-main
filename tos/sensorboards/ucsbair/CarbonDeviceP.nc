configuration CarbonDeviceP {
  provides {
    interface Atm128AdcConfig;
  }
}
implementation {
  components CarbonP, MicaBusC;

  Atm128AdcConfig = CarbonP;

  CarbonP.CarbonAdc -> MicaBusC.Adc1;
}
