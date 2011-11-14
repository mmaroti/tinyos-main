
configuration RadioStackC {
	provides {
		interface RadioState;
		interface RadioSend;
		interface RadioPacket;
		interface RadioReceive;
	}
}

implementation {
	#define UQ_RADIO_ALARM	"UQ_SI443X_RADIO_ALARM"

	RadioState = RadioDriverLayerC;
	RadioSend = RadioDriverLayerC;
	RadioPacket = RadioDriverLayerC;
	RadioReceive = RadioDriverLayerC;
	
	components RadioStackP;
	components new RadioAlarmC();
	
	RadioAlarmC.Alarm -> RadioDriverLayerC;
	
	components Si443xDriverLayerC as RadioDriverLayerC;
	RadioDriverLayerC.Config -> RadioStackP;
	RadioDriverLayerC.PacketTimeStamp -> RadioStackP;
	
	RadioDriverLayerC.TransmitPowerFlag -> RadioStackP.TransmitPowerFlag;
	RadioDriverLayerC.RSSIFlag -> RadioStackP.RSSIFlag;
	RadioDriverLayerC.TimeSyncFlag -> RadioStackP.TimeSyncFlag;
	
	RadioDriverLayerC.RadioAlarm -> RadioAlarmC.RadioAlarm[unique(UQ_RADIO_ALARM)];

}
