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
    interface SplitControl as RadioControl;
    interface Packet as RadioPacket;
    interface Receive as RadioReceive;
    
    interface PacketField<uint8_t> as PacketTransmitPower;
		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketTimeSyncOffset;
		interface PacketField<uint8_t> as PacketLinkQuality;
		
		interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
		interface PacketTimeStamp<TRadio, uint32_t> as PacketTimeStampRadio;		
  }
}

implementation {


// -------- Some component transformation hacks

  components RadioPacket2PacketP;
  RadioPacket2PacketP.RadioPacket -> RadioDriverLayerC;
  RadioPacket = RadioPacket2PacketP;
  
  components RadioReceive2ReceiveP;
  RadioReceive2ReceiveP.RadioReceive -> RadioDriverLayerC;
  RadioReceive2ReceiveP.RadioPacket  -> RadioDriverLayerC;
  RadioReceive = RadioReceive2ReceiveP;
  
  components RadioState2SplitControlP;
  RadioState2SplitControlP.RadioState -> RadioDriverLayerC;
  RadioControl = RadioState2SplitControlP;

// -------- TX power, LQI, RSSI, Timestamps

  PacketTransmitPower   = RadioDriverLayerC.PacketTransmitPower;
	PacketRSSI            = RadioDriverLayerC.PacketRSSI;
	PacketTimeSyncOffset  = RadioDriverLayerC.PacketTimeSyncOffset;
  PacketLinkQuality     = RadioDriverLayerC.PacketLinkQuality;

	PacketTimeStampRadio  = TimeStampingLayerC;
	PacketTimeStampMilli  = TimeStampingLayerC;

// -------- TimeStamping

	components TimeStampingLayerC;
	TimeStampingLayerC.LocalTimeRadio -> RadioDriverLayerC;
	TimeStampingLayerC.SubPacket      -> MetadataFlagsLayerC;

// -------- MetadataFlags

	components MetadataFlagsLayerC;
	MetadataFlagsLayerC.SubPacket -> RadioDriverLayerC;

// -------- Timestamping Layer

  RadioDriverLayerC.PacketTimeStamp -> TimeStampingLayerC;

// -------- Radio Driver

  components RF230DriverLayerC as RadioDriverLayerC;
  components RF230RadioP;
  RadioDriverLayerC.Config -> RF230RadioP;
}
