#include "UcminiSensor.h"
configuration UcminiSensorC { }
implementation {
  components UcminiSensorP, MainC, LedsC, new TimerMilliC();
  components new AtmegaTemperatureC(), new AtmegaVoltageC(),
             new LightC(),
             new PressureC(), new Temperature1C(),
             new TemperatureC(), new HumidityC();
  components SerialStartC, new SerialAMSenderC(AM_MEASUREMENT) as MeasSend, new SerialAMSenderC(AM_CALIB) as CalibSend;

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
  UcminiSensorP.MeasSend->MeasSend;
  UcminiSensorP.CalibSend->CalibSend;
  UcminiSensorP.Packet->MeasSend;
  UcminiSensorP.Leds -> LedsC;
}
