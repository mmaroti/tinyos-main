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

#include "Packet.h"

configuration RSSINodeAppC 
{ 
} 
implementation { 
  
  	components RSSINodeC as App;
  	components MainC;
  	components LedsC;
  	components ActiveMessageC;
  	components new TimerMilliC() as MesTimer;
  	components new TimerMilliC() as ledTimer;
	components new VoltageC();

#ifdef PLATFORM_IRIS
	components RF230ActiveMessageC;
	App.PacketRSSI->RF230ActiveMessageC.PacketRSSI;
	App.PacketLinkQuality->RF230ActiveMessageC.PacketLinkQuality;
	
#else
	components CC2420ActiveMessageC;
	App.CC2420Packet->CC2420ActiveMessageC.CC2420Packet;
	App.intfSendInt8 -> ActiveMessageC.AMSend[AM_DATAINT8];
#endif
#ifdef PLATFORM_TELOSB
	components UserButtonC;
	components new SensirionSht11C();
	components new HamamatsuS1087ParC();
	components new HamamatsuS10871TsrC();
	App.Temp -> SensirionSht11C.Temperature;
	App.Hum -> SensirionSht11C.Humidity;
	App.RadSens ->	HamamatsuS1087ParC.Read;
	App.OvRadSens -> HamamatsuS10871TsrC.Read;
	App.Notify -> UserButtonC;
#endif
  	App.Boot -> MainC;
  	App.Leds -> LedsC;
  	App.Vref->VoltageC;
  	App.MesTimer->MesTimer;
  	App.ledTimer->ledTimer;
  	App.SplitControl ->ActiveMessageC;
  	App.Receive -> ActiveMessageC.Receive[AM_CONTROLPACKET];
  	App.intfSendUint8 -> ActiveMessageC.AMSend[AM_DATAUINT8];
	App.intfSendUint16 -> ActiveMessageC.AMSend[AM_DATAUINT16];
}

