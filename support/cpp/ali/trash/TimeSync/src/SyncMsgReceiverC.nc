
#include "SyncMsg.h"

configuration SyncMsgReceiverC {
	
}

implementation {
	
	components TimeSyncMessageC;
	components LedsC;
	components SyncMsgReceiverP;
	
	SyncMsgReceiverP.Receive -> TimeSyncMessageC.Receive[AM_SYNCMSG];
	SyncMsgReceiverP.AMPacket -> TimeSyncMessageC;
	SyncMsgReceiverP.TimeSyncPacket -> TimeSyncMessageC.TimeSyncPacketMilli;
	SyncMsgReceiverP.Leds -> LedsC;	
}