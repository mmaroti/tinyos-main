configuration ApplicationC{
}
implementation{
	components new SensorMts300C() as Sensor, new TimerMilliC();
	components StorageFrameC, MainC, ApplicationP, LedsC, LocalTimeMilliC;
	components StreamUploaderC, ActiveMessageC;
	
	ApplicationP.Boot->MainC;
	ApplicationP.Light->Sensor.Light;
	ApplicationP.Temp->Sensor.Temp;
	ApplicationP.Vref->Sensor.Vref;
	ApplicationP.StreamStorageWrite->StorageFrameC;
	ApplicationP.Leds->LedsC;	
	ApplicationP.Timer->TimerMilliC;
	ApplicationP.LocalTime->LocalTimeMilliC;
	ApplicationP.RadioControl->ActiveMessageC;

	components RF230ActiveMessageC as LplRadio, SystemLowPowerListeningC;
	ApplicationP.LPL -> LplRadio.LowPowerListening;
	ApplicationP.SysLPL -> SystemLowPowerListeningC.SystemLowPowerListening;	
}