configuration SyncMsgSenderC {
	
	provides interface SyncMsgSender;
}

implementation {
	
	components CounterMilli32C as Counter;
	components new CounterToLocalTimeC(TMilli);
	components TimeSyncMessageC;
	components SyncMsgSenderP;
	
	CounterToLocalTimeC.Counter -> Counter;
	SyncMsgSenderP.LocalTime -> CounterToLocalTimeC;
	SyncMsgSenderP.TimeSyncAMSend -> TimeSyncMessageC.TimeSyncAMSendMilli[AM_ID];
	SyncMsgSenderP.Packet -> TimeSyncMessageC;
	SyncMsgSender = SyncMsgSenderP;
	
}
