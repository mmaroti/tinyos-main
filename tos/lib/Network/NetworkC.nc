#include "Scheduler.h"
#include "Network.h"
configuration NetworkC{
	provides interface StdControl;
	provides interface TimeSyncPoints;
	provides interface AMSend[am_id_t id];
	provides interface Receive[am_id_t id];
	provides interface Receive as Snoop[am_id_t id];
}
implementation{
	components NetSchedulersC;
	components RadioSchedulerP, TimeSyncMessageC, LedsC, NetworkP, ActiveMessageC,RandomC, new TimerMilliC();
	RadioSchedulerP.SplitControl->TimeSyncMessageC;
	RadioSchedulerP.Leds->LedsC;
	
	NetworkP.TimeSyncAMSend -> TimeSyncMessageC.TimeSyncAMSendMilli[10];
//	NetworkP.TimeSyncReceive -> TimeSyncMessageC.Receive[10];
	NetworkP.TimeSyncPacket -> TimeSyncMessageC;
	NetworkP.TimeSyncAMPacket -> TimeSyncMessageC.AMPacket;
	
	NetworkP.Packet->ActiveMessageC;
	NetworkP.AMPacket->ActiveMessageC;
	NetworkP.SubSend->ActiveMessageC.AMSend;
	NetworkP.SubReceive->ActiveMessageC.Receive;
	NetworkP.SubSnoop->ActiveMessageC.Snoop;
	NetworkP.PacketAcknowledgements->ActiveMessageC;
	NetworkP.LocalTime->NetSchedulersC.LocalTime;
	NetworkP.DiscoveryScheduler->NetSchedulersC.DiscoveryScheduler;
	NetworkP.BeaconScheduler->NetSchedulersC.BeaconScheduler;
	NetworkP.NetworkScheduler->NetSchedulersC.NeighborSchedulers;
	NetworkP.Random->RandomC;
	NetworkP.RandomInit->RandomC;
	NetworkP.PacketTimeStampMilli-> TimeSyncMessageC;
	NetworkP.Leds->LedsC;
	NetworkP.Timer->TimerMilliC;
	StdControl=NetworkP.StdControl;
	TimeSyncPoints=NetworkP.TimeSyncPoints;
	AMSend=NetworkP.AMSend;
	Receive=NetworkP.Receive;
	Snoop=NetworkP.Snoop;
 
}