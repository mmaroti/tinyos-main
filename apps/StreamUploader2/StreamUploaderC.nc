#include "StreamUploader.h"
#include "StreamStorage.h"
configuration StreamUploaderC{
}
implementation{
	components StreamUploaderP, ActiveMessageC, LedsC;
	components new AMSenderC(AM_CTRL_MSG) as CtrlSend, new AMReceiverC(AM_GET_MSG) as GetRecieve, new AMReceiverC(AM_COMMAND_MSG) as CommandReceive, new AMSenderC(AM_DATA_MSG) as DataSend;
	components new TimerMilliC(), new StreamStorageClientC();
	
	StreamUploaderP.DataSend->DataSend;
	StreamUploaderP.CtrlSend->CtrlSend;
	StreamUploaderP.GetReceive->GetRecieve;
	StreamUploaderP.CommandReceive->CommandReceive;      
	StreamUploaderP.Packet->ActiveMessageC;
	StreamUploaderP.AMPacket->ActiveMessageC;
	StreamUploaderP.StreamStorageRead->StreamStorageClientC;
	StreamUploaderP.StreamStorageErase->StreamStorageClientC;
	StreamUploaderP.Resource->StreamStorageClientC;
	StreamUploaderP.Timer->TimerMilliC;
	StreamUploaderP.Leds->LedsC;
}