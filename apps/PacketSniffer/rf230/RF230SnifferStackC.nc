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
* Author: Veress Krisztian
*         veresskrisztian@gmail.com
*/

configuration RF230SnifferStackC {
  provides {
    interface SplitControl;
    interface Receive;
    interface Packet;
    interface SnifferData;
  }
}

implementation {


// -------- Some interface transformation hacks

  components InterfaceTransformerP;
  InterfaceTransformerP.BareReceive -> MessageBufferLayerC.Receive;
  InterfaceTransformerP.RadioPacket  -> RadioDriverLayerC;
  InterfaceTransformerP.BareSend -> MessageBufferLayerC.Send;
  
  Receive = InterfaceTransformerP;
  Packet = InterfaceTransformerP;
  
// -------- RSSI, LQI, Timestamp

  components SnifferDataP;
  SnifferDataP.PacketRSSI -> RadioDriverLayerC.PacketRSSI;
  SnifferDataP.PacketLQI -> RadioDriverLayerC.PacketLinkQuality;
  SnifferDataP.Timestamp -> TimeStampingLayerC.PacketTimeStampRadio;
  
  SnifferData = SnifferDataP;

// -------- Message Buffering

	components MessageBufferLayerC;
	MessageBufferLayerC.RadioSend -> RadioDriverLayerC;
	MessageBufferLayerC.RadioReceive -> RadioDriverLayerC;
	MessageBufferLayerC.RadioState -> RadioDriverLayerC;
	
  SplitControl = MessageBufferLayerC;

// -------- IEEE 802.15.4 

  components Ieee154PacketLayerC;
	Ieee154PacketLayerC.SubPacket -> TimeStampingLayerC;

// -------- TimeStamping

  components TimeStampingLayerC;
  TimeStampingLayerC.LocalTimeRadio -> RadioDriverLayerC;
  TimeStampingLayerC.SubPacket      -> MetadataFlagsLayerC;

// -------- MetadataFlags

  components MetadataFlagsLayerC;
  MetadataFlagsLayerC.SubPacket -> RadioDriverLayerC;

// -------- Radio Driver

  components RF230DriverLayerC as RadioDriverLayerC;
  components RF230RadioP, RadioAlarmC;
  
  RF230RadioP.Ieee154PacketLayer -> Ieee154PacketLayerC;
	RF230RadioP.RadioAlarm -> RadioAlarmC.RadioAlarm[unique("RadioAlarm")];
	RF230RadioP.PacketTimeStamp -> TimeStampingLayerC;
	RF230RadioP.RF230Packet -> RadioDriverLayerC;
  
  RadioDriverLayerC.Config -> RF230RadioP;
  RadioDriverLayerC.PacketTimeStamp -> TimeStampingLayerC;
}
