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
	components RadioSchedulerP, TimeSyncMessageC, LedsC, NetworkP, ActiveMessageC,RandomC;
	components new TimerMilliC() as ListenTimer, new TimerMilliC() as FirstSendTimer;
	RadioSchedulerP.SplitControl->TimeSyncMessageC;
	RadioSchedulerP.Leds->LedsC;
	
	NetworkP.TimeSyncAMSend -> TimeSyncMessageC.TimeSyncAMSendMilli[BEACON_AM_ID];
	NetworkP.TimeSyncReceive -> TimeSyncMessageC.Receive[BEACON_AM_ID];
	NetworkP.TimeSyncPacket -> TimeSyncMessageC;
	NetworkP.TimeSyncAMPacket -> TimeSyncMessageC.AMPacket;
	
	NetworkP.Packet->ActiveMessageC;
	NetworkP.AMPacket->ActiveMessageC;
	NetworkP.SubSend->ActiveMessageC.AMSend[DATA_AM_ID];
	NetworkP.SubReceive->ActiveMessageC.Receive[DATA_AM_ID];
	NetworkP.SubSnoop->ActiveMessageC.Snoop[DATA_AM_ID];
	NetworkP.LocalTime->NetSchedulersC.LocalTime;
	NetworkP.DiscoveryScheduler->NetSchedulersC.DiscoveryScheduler;
	NetworkP.BeaconScheduler->NetSchedulersC.BeaconScheduler;
	NetworkP.NetworkScheduler->NetSchedulersC.NeighborSchedulers;
	NetworkP.Random->RandomC;
	NetworkP.RandomInit->RandomC;
	NetworkP.PacketTimeStampMilli-> TimeSyncMessageC;
	NetworkP.Leds->LedsC;
	NetworkP.ListenTimer->ListenTimer;
	NetworkP.FirstSendTimer->FirstSendTimer;
	StdControl=NetworkP.StdControl;
	TimeSyncPoints=NetworkP.TimeSyncPoints;
	AMSend=NetworkP.AMSend;
	Receive=NetworkP.Receive;
	Snoop=NetworkP.Snoop;
 
}