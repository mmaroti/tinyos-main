/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

#include "TimeSyncPoints.h"


configuration TimeSyncPointsC
{
	provides interface TimeSyncPoints;
	provides interface StdControl;
    provides interface Set<uint16_t> as SetInterval;
    provides interface Set<uint8_t> as SetSavePoints;
    provides interface Get<uint8_t> as GetLastRSSI;
    provides interface Get<uint8_t> as GetLastLQI;
}
implementation
{
	components TimeSyncMessageC, LocalTimeMilliC, new EepromVariableUint16P(0);
  	components TimeSyncPointsP;
		#if defined(PLATFORM_IRIS)
		components RF230ActiveMessageC as RadioActiveMessageC;
		#elif defined(PLATFORM_UCMINI)
		components RFA1ActiveMessageC as RadioActiveMessageC;
		#endif
  	components new TimerMilliC();

 	TimeSyncPointsP.PacketTimeStampMilli -> TimeSyncMessageC.PacketTimeStampMilli;
  	TimeSyncPointsP.TimeSyncReceive -> TimeSyncMessageC.Receive[AM_TIMESYNCPOINTS]; 
  	TimeSyncPointsP.TimeSyncPacketMilli -> TimeSyncMessageC.TimeSyncPacketMilli;
  	TimeSyncPointsP.TimeSyncAMSendMilli -> TimeSyncMessageC.TimeSyncAMSendMilli[AM_TIMESYNCPOINTS];
  	TimeSyncPointsP.Timer -> TimerMilliC;
    TimeSyncPointsP.AMPacket -> TimeSyncMessageC.AMPacket;
    TimeSyncPointsP.LocalTime -> LocalTimeMilliC;
    TimeSyncPointsP.PacketRSSI->RadioActiveMessageC.PacketRSSI;
    TimeSyncPointsP.PacketLinkQuality->RadioActiveMessageC.PacketLinkQuality;
    TimeSyncPointsP.getBootCount->EepromVariableUint16P;
  	TimeSyncPoints=TimeSyncPointsP;
  	StdControl=TimeSyncPointsP;
    SetInterval=TimeSyncPointsP;
    SetSavePoints=TimeSyncPointsP;
    GetLastRSSI=TimeSyncPointsP.GetLastRSSI;
    GetLastLQI=TimeSyncPointsP.GetLastLQI;
    
    
}

