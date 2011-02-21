configuration ApplicationC{
}
implementation{
	components new TimerMilliC();
	components StorageFrameC, MainC, ApplicationP, LedsC, LocalTimeMilliC;
	components StreamUploaderC, ActiveMessageC;
	components new SensirionSht11C(), new Taos2550C();
	
	ApplicationP.Boot->MainC;
	ApplicationP.Temp->SensirionSht11C.Temperature;
	ApplicationP.Humidity->SensirionSht11C.Humidity;
	ApplicationP.VLight->Taos2550C.VisibleLight;
	ApplicationP.IRLight->Taos2550C.InfraredLight;
	ApplicationP.StreamStorageWrite->StorageFrameC;
	ApplicationP.Leds->LedsC;	
	ApplicationP.Timer->TimerMilliC;
	ApplicationP.LocalTime->LocalTimeMilliC;
	ApplicationP.RadioControl->ActiveMessageC;

	components RF230ActiveMessageC as LplRadio, SystemLowPowerListeningC;
	ApplicationP.LPL -> LplRadio.LowPowerListening;
	ApplicationP.SysLPL -> SystemLowPowerListeningC.SystemLowPowerListening;	
}