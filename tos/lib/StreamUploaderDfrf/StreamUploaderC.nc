#include "StreamUploader.h"
#include "StreamStorage.h"
configuration StreamUploaderC{
  provides interface Command;
  provides interface Debug;
}
implementation{
    components StreamUploaderP, ActiveMessageC,ConvergecastC, MainC, new StreamStorageClientC();
    
    components GradientPolicyC, SpanningTreePolicyC;
    components new DfrfClientC(AM_CTRL_MSG, sizeof(ctrl_msg), sizeof(ctrl_msg), 15) as DfrfCtrl;
    components new DfrfClientC(AM_DATA_MSG, sizeof(data_msg), sizeof(data_msg), 15) as DfrfData;

    components new BroadcastClientC(AM_GET_MSG, sizeof(get_msg)) as DfrfGet;
    components new BroadcastClientC(AM_COMMAND_MSG, sizeof(command_msg)) as DfrfCommand;
    components NoLedsC as LedsC, new TimerMilliC();
    
    DfrfCtrl.DfrfPolicy->GradientPolicyC;
    DfrfData.DfrfPolicy->SpanningTreePolicyC;
    
    StreamUploaderP.DataSend->DfrfData;
    StreamUploaderP.CtrlSend->DfrfCtrl;
    StreamUploaderP.DataReceive->DfrfData;
    StreamUploaderP.CtrlReceive->DfrfCtrl;
    StreamUploaderP.GetReceive->DfrfGet;
    StreamUploaderP.CommandReceive->DfrfCommand; 
    StreamUploaderP.ConvergecastInit->ConvergecastC;  
      
    StreamUploaderP.StreamStorageRead->StreamStorageClientC;
    StreamUploaderP.StreamStorageErase->StreamStorageClientC;
    StreamUploaderP.Resource->StreamStorageClientC;
    StreamUploaderP.Leds->LedsC;
    StreamUploaderP.Boot->MainC;
    StreamUploaderP.Timer->TimerMilliC;
    Command=StreamUploaderP;
    Debug=StreamUploaderP;
}