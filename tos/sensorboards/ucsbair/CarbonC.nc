generic configuration CarbonC() {
  provides interface Read<uint16_t>;
}
implementation {
  components new AdcReadClientC(),CarbonDeviceP;

  Read = AdcReadClientC;
  AdcReadClientC.Atm128AdcConfig -> CarbonDeviceP;
}
