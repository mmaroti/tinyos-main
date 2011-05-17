configuration ApplicationC{
}
implementation{
	components new TimerMilliC() as SensorTimer, new TimerMilliC() as SystemTimer, new EepromVariableUint16P(0);
	components new StorageFrameC() as DataStor, new StorageFrameC() as TimeStor, MainC, ApplicationP, LedsC, LocalTimeMilliC;
	components StreamUploaderC, ActiveMessageC, TimeSyncPointsC;
	components new SensirionSht11C(), new Taos2550C(), new VoltageC(), new AMReceiverC(20);
	
	ApplicationP.Boot->MainC;
	ApplicationP.Temp->SensirionSht11C.Temperature;
	ApplicationP.Humidity->SensirionSht11C.Humidity;
	ApplicationP.VLight->Taos2550C.VisibleLight;
	ApplicationP.IRLight->Taos2550C.InfraredLight;
    ApplicationP.VRef->VoltageC;
	ApplicationP.DataStorageWrite->DataStor;
    ApplicationP.TimeStorageWrite->TimeStor;
	ApplicationP.Leds->LedsC;	
	ApplicationP.SystemTimer->SensorTimer;
    ApplicationP.SensorTimer->SystemTimer;
	ApplicationP.LocalTime->LocalTimeMilliC;
	ApplicationP.RadioControl->ActiveMessageC;
    ApplicationP.SetInterval->TimeSyncPointsC;
    ApplicationP.SetSavePoints->TimeSyncPointsC;
    ApplicationP.GetLastRSSI->TimeSyncPointsC.GetLastRSSI;
    ApplicationP.GetLastLQI->TimeSyncPointsC.GetLastLQI;
    ApplicationP.TimeSyncPoints->TimeSyncPointsC;
    ApplicationP.TimeSync->TimeSyncPointsC;
    ApplicationP.Command->StreamUploaderC;
    ApplicationP.EepromWrite->EepromVariableUint16P;
    ApplicationP.EepromRead->EepromVariableUint16P;
    ApplicationP.Receive->AMReceiverC;
    
    components StreamStorageC;
    ApplicationP.StorDebug->StreamStorageC;
    ApplicationP.DataDebug->DataStor;
    ApplicationP.TimeDebug->TimeStor;
    ApplicationP.UplDebug->StreamUploaderC;

	components RF230ActiveMessageC as LplRadio, SystemLowPowerListeningC;
	ApplicationP.LPL -> LplRadio.LowPowerListening;
	ApplicationP.SysLPL -> SystemLowPowerListeningC.SystemLowPowerListening;	
}