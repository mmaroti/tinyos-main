
configuration MainAppC {
		
}

implementation{
	
	components MainC;
	components MainAppP;	
	components AssertC;	
	components new TimerMilliC() as TimerMilli;
	components TimeSyncMessageC;
	components LedsC;
	components SyncMsgSenderC;
	components SyncMsgReceiverP;

	MainAppP.Boot -> MainC;
	MainAppP.Timer -> TimerMilli;
	MainAppP.TimeSyncMsg -> TimeSyncMessageC.SplitControl;
	MainAppP.Leds -> LedsC;
	MainAppP.SyncMsgSender -> SyncMsgSenderC;
	
}
