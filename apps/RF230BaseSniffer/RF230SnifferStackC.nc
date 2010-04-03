configuration RF230SnifferStackC {
  provides {
    interface RadioState;
    interface RadioPacket;
    interface RadioReceive;
  }
}

implementation {

  RadioState = RF230DriverLayerC;  
  RadioPacket = RF230DriverLayerC;
  RadioReceive = RF230DriverLayerC;

/*  components RF230RadioP,RadioAlarmC;
  RF230RadioP.Ieee154PacketLayer -> Ieee154PacketLayerC;
	RF230RadioP.RadioAlarm -> RadioAlarmC.RadioAlarm[unique("RadioAlarm")];
	RF230RadioP.PacketTimeStamp -> TimeStampingLayerC;
	RF230RadioP.RF230Packet -> RF230DriverLayerC;

  RF230BaseSnifferP.PacketTransmitPower = RF230DriverLayerC.PacketTransmitPower;
  RF230BaseSnifferP.PacketLinkQuality = RF230DriverLayerC.PacketLinkQuality;
	RF230BaseSnifferP.PacketRSSI = RF230DriverLayerC.PacketRSSI;
	RF230BaseSnifferP.PacketTimeStampRadio = TimeStampingLayerC;
	RF230BaseSnifferP.PacketTimeStampMilli = TimeStampingLayerC;

// -------- Active Message

	components ActiveMessageLayerC;
	ActiveMessageLayerC.Config -> RF230RadioP;
	ActiveMessageLayerC.SubSend -> MessageBufferLayerC;
	ActiveMessageLayerC.SubReceive -> MessageBufferLayerC;
	ActiveMessageLayerC.SubPacket -> TimeStampingLayerC;

// -------- IEEE 802.15.4 Packet

	components Ieee154PacketLayerC;
	Ieee154PacketLayerC.SubPacket -> TimeStampingLayerC;

	components MessageBufferLayerC;
	MessageBufferLayerC.RadioSend -> RF230DriverLayerC;
	MessageBufferLayerC.RadioReceive -> AckDetectionLayerC.Receive;
	MessageBufferLayerC.RadioState -> RF230DriverLayerC;

// -------- Acknowledgement Detection
  components AckDetectionLayerC;
  AckDetectionLayerC.Config -> RF230RadioP;
  AckDetectionLayerC.SubReceive -> RF230DriverLayerC;
*/

// -------- TimeStamping

	components TimeStampingLayerC;
	TimeStampingLayerC.LocalTimeRadio -> RF230DriverLayerC;
	TimeStampingLayerC.SubPacket -> MetadataFlagsLayerC;

// -------- MetadataFlags

	components MetadataFlagsLayerC;
	MetadataFlagsLayerC.SubPacket -> RF230DriverLayerC;

// -------- RF230 Driver
	components RF230DriverLayerC, RF230RadioP;
	RF230DriverLayerC.Config -> RF230RadioP;
	RF230DriverLayerC.PacketTimeStamp -> TimeStampingLayerC;

}
