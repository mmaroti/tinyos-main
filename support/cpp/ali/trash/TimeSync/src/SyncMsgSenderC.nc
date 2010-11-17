configuration SyncMsgSenderC {
	
	provides interface SyncMsgSender;
}

implementation {
	
	components SyncMsgSenderP;
	
	SyncMsgSender = SyncMsgSenderP;

	components CounterMilli32C as Counter;
	components new CounterToLocalTimeC(TMilli);
	
	CounterToLocalTimeC.Counter -> Counter;
	SyncMsgSenderP.LocalTime -> CounterToLocalTimeC;
	
	components TimeSyncMessageC;
	
	SyncMsgSenderP.TimeSyncAMSend -> TimeSyncMessageC.TimeSyncAMSendMilli[AM_ID];
	SyncMsgSenderP.Packet -> TimeSyncMessageC;
}
