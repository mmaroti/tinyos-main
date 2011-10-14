configuration UcminiSensorC { }
implementation {
  components UcminiSensorP, MainC, Sht21C, Bh1750fviC, Ms5607C, LedsC;
  components DiagMsgC, new TimerMilliC();

  UcminiSensorP.Boot -> MainC;
  UcminiSensorP.TempRead -> Sht21C.Temperature;
  UcminiSensorP.HumiRead -> Sht21C.Humidity;
  UcminiSensorP.LightRead -> Bh1750fviC.Light;
  UcminiSensorP.PressRead -> Ms5607C.ReadPressure;
  UcminiSensorP.Temp2Read -> Ms5607C.ReadTemperature;
  UcminiSensorP.ReadRef -> Ms5607C.ReadCalibration;
  UcminiSensorP.Timer->TimerMilliC;
  UcminiSensorP.DiagMsg -> DiagMsgC;
  UcminiSensorP.Leds -> LedsC;
}
