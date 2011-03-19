/*
* Copyright (c) 2009, University of Szeged
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
* Author: Miklos Maroti
*/

configuration RF230RipsEngineC
{
	provides
	{
		interface RadioSend;
		interface RadioReceive;
	}
	uses
	{
		interface RadioSend as SubSend;
		interface RadioReceive as SubReceive;

		interface RadioRegister;
		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketLinkQuality;
	}
}

implementation
{
	components RF230RipsEngineP, RadioAlarmC, ActiveMessageAddressC, TaskletC, ActiveMessageLayerP;

	RF230RipsEngineP.RadioAlarm -> RadioAlarmC.RadioAlarm[unique("RadioAlarm")];
	RF230RipsEngineP.PayloadPacket -> ActiveMessageLayerP;
	RF230RipsEngineP.ActiveMessageAddress -> ActiveMessageAddressC;
	RF230RipsEngineP.Tasklet -> TaskletC;

	RadioSend = RF230RipsEngineP;
	SubSend = RF230RipsEngineP;
	RadioReceive = RF230RipsEngineP;
	SubReceive = RF230RipsEngineP;

	RadioRegister = RF230RipsEngineP;
	PacketRSSI = RF230RipsEngineP.PacketRSSI;
	PacketLinkQuality = RF230RipsEngineP.PacketLinkQuality;

#ifdef RADIO_DEBUG
	components DiagMsgC, LedsC;
	RF230RipsEngineP.DiagMsg -> DiagMsgC;
	RF230RipsEngineP.Leds -> LedsC;
#endif
}
