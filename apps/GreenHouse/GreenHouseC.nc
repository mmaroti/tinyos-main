#include "GreenHouse.h"

configuration GreenHouseC{
}
implementation{

	components GreenHouseP, MainC;
	components LedsC;
	components CollectionC as Collector;
	components new CollectionSenderC(CL_TEST);
	components new TimerMilliC();
	components new SensirionSht11C();
	
	components SerialActiveMessageC;
	components ActiveMessageC;
	
	components new AMSenderC(AM_GREENHOUSE);
	
	components DiagMsgC;
	
	GreenHouseP.Boot -> MainC;
	GreenHouseP.Leds -> LedsC;
	GreenHouseP.Timer -> TimerMilliC;
	GreenHouseP.Read -> SensirionSht11C.Temperature;
	
	GreenHouseP.CtpControl -> Collector;
	GreenHouseP.RootControl -> Collector;
	GreenHouseP.SerialControl -> SerialActiveMessageC;
	GreenHouseP.RadioControl -> ActiveMessageC;
	
	GreenHouseP.CollectionPacket -> Collector;
	GreenHouseP.CtpInfo -> Collector;
	GreenHouseP.CtpCongestion -> Collector;
	
	GreenHouseP.CtpSend -> CollectionSenderC;
	GreenHouseP.CtpReceive -> Collector.Receive[CL_TEST];
	
	GreenHouseP.SerialSend -> AMSenderC;
	
	GreenHouseP.DiagMsg -> DiagMsgC;
}