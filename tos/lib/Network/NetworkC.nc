/*
* Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author:Andras Biro
*/
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