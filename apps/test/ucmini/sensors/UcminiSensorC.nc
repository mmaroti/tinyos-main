configuration UcminiSensorC { }
implementation {
  components UcminiSensorP, MainC, new TimerMilliC();
  components new AtmegaTemperatureC(), new AtmegaVoltageC(),
             Sht21C, Bh1750fviC;
   components new Ms5607RawPressureC(), new Ms5607RawTemperatureC();
  components DiagMsgC, LedsC;

  UcminiSensorP.Boot -> MainC;
  UcminiSensorP.TempRead -> Sht21C.Temperature;
  UcminiSensorP.HumiRead -> Sht21C.Humidity;
  UcminiSensorP.LightRead -> Bh1750fviC.Light;
  UcminiSensorP.PressRead -> Ms5607RawPressureC;
  UcminiSensorP.Temp2Read -> Ms5607RawTemperatureC;
  UcminiSensorP.ReadRef -> Ms5607RawPressureC.ReadCalibration;
  UcminiSensorP.Temp3Read -> AtmegaTemperatureC;
  UcminiSensorP.VoltageRead -> AtmegaVoltageC;
  UcminiSensorP.Timer->TimerMilliC;
  UcminiSensorP.DiagMsg -> DiagMsgC;
  UcminiSensorP.Leds -> LedsC;
}
