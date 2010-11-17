
#include "SyncMsg.h"

configuration SyncMsgReceiverC {
	
}

implementation {
	
	components TimeSyncMessageC;
	components SyncMsgReceiverP;
	
	SyncMsgReceiverP.Receive -> TimeSyncMessageC.Receive[AM_ID];
	SyncMsgReceiverP.AMPacket -> TimeSyncMessageC;
	SyncMsgReceiverP.TimeSyncPacket -> TimeSyncMessageC.TimeSyncPacketMilli[AM_ID];
	SyncMsgReceiverP.PacketTimeStamp -> TimeSyncMessageC.PacketTimeStampMilli;
}