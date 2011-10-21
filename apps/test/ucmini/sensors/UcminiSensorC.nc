configuration UcminiSensorC { }
implementation {
  components UcminiSensorP, MainC, new TimerMilliC();
  components new AtmegaTemperatureC(), new AtmegaVoltageC(),
             new LightC();
  components new PressureC(), new Temperature1C();
  components new TemperatureC(), new HumidityC();
  components DiagMsgC, LedsC;

  UcminiSensorP.Boot -> MainC;
  UcminiSensorP.TempRead -> TemperatureC;
  UcminiSensorP.HumiRead -> HumidityC;
  UcminiSensorP.LightRead -> LightC;
  UcminiSensorP.PressRead -> PressureC;
  UcminiSensorP.Temp2Read -> Temperature1C;
  UcminiSensorP.ReadRef -> PressureC.ReadCalibration;
  UcminiSensorP.Temp3Read -> AtmegaTemperatureC;
  UcminiSensorP.VoltageRead -> AtmegaVoltageC;
  UcminiSensorP.Timer->TimerMilliC;
  UcminiSensorP.DiagMsg -> DiagMsgC;
  UcminiSensorP.Leds -> LedsC;
}
