configuration ApplicationC{
}
implementation{
	components new TimerMilliC();
	components new StorageFrameC(), MainC, ApplicationP, LedsC, LocalTimeMilliC;
	components StreamUploaderC, ActiveMessageC, TimeSyncPointsC;
	components new SensirionSht11C(), new Taos2550C();
	
	ApplicationP.Boot->MainC;
	ApplicationP.Temp->SensirionSht11C.Temperature;
	ApplicationP.Humidity->SensirionSht11C.Humidity;
	ApplicationP.VLight->Taos2550C.VisibleLight;
	ApplicationP.IRLight->Taos2550C.InfraredLight;
	ApplicationP.DataStorageWrite->StorageFrameC;
    ApplicationP.TimeStorageWrite->StorageFrameC;
	ApplicationP.Leds->LedsC;	
	ApplicationP.Timer->TimerMilliC;
	ApplicationP.LocalTime->LocalTimeMilliC;
	ApplicationP.RadioControl->ActiveMessageC;
    ApplicationP.SetInterval->TimeSyncPointsC;
    ApplicationP.TimeSyncPoints->TimeSyncPointsC;
    ApplicationP.TimeSync->TimeSyncPointsC;
    ApplicationP.Command->StreamUploaderC;

	components RF230ActiveMessageC as LplRadio, SystemLowPowerListeningC;
	ApplicationP.LPL -> LplRadio.LowPowerListening;
	ApplicationP.SysLPL -> SystemLowPowerListeningC.SystemLowPowerListening;	
}