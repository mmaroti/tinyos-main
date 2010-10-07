module CarbonP {
  //provides interface ResourceConfig;
  provides interface Atm128AdcConfig;

  uses interface MicaBusAdc as CarbonAdc;
}
implementation {
  async command uint8_t Atm128AdcConfig.getChannel() {
    return call CarbonAdc.getChannel();
  }

  async command uint8_t Atm128AdcConfig.getRefVoltage() {
    return ATM128_ADC_VREF_OFF;
  }

  async command uint8_t Atm128AdcConfig.getPrescaler() {
    return ATM128_ADC_PRESCALE;
  }
}
