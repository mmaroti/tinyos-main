
configuration MainAppC {
		
}

implementation{
	
	components MainC;
	components MainAppP;	
	//components ActiveMessageC;	
	components AssertC;	
	components new TimerMilliC() as TimerMilli;
	components TimeSyncMessageC;
	components LedsC;
	components SyncMsgSenderC;

	MainAppP.Boot -> MainC;
	//MainAppP.Radio -> ActiveMessageC.SplitControl;
	MainAppP.Timer -> TimerMilli;
	MainAppP.TimeSyncMsg -> TimeSyncMessageC.SplitControl;
	MainAppP.Leds -> LedsC;
	MainAppP.SyncMsgSender -> SyncMsgSenderC;
	
}
