#include "GreenHouse.h"

configuration GreenHouseC{
}
implementation{

	components GreenHouseP, MainC;
	components LedsC;
	components new TimerMilliC();
	components new SensirionSht11C();
	
	components CollectionC as Collector;
	components new CollectionSenderC(CL_TEST);
	
	//components new AMSenderC(AM_GREENHOUSE);
	
	components ActiveMessageC;
	components SerialActiveMessageC;
	
	components new QueueC(message_t*, 10) as QueueCtp;
	components new QueueC(message_t*, 10) as QueueSerial;
	components new QueueC(message_t*, 10) as QueueTemp;
	
	components DiagMsgC;
	
	
	GreenHouseP.Boot	-> MainC;
	GreenHouseP.Leds	-> LedsC;
	GreenHouseP.Timer	-> TimerMilliC;
	GreenHouseP.Read	-> SensirionSht11C.Temperature;
	
	GreenHouseP.CtpControl		-> Collector;
	GreenHouseP.RootControl		-> Collector;
	GreenHouseP.CollectionPacket-> Collector;
	GreenHouseP.CtpInfo 		-> Collector;
	GreenHouseP.CtpCongestion 	-> Collector;
	GreenHouseP.CtpReceive 		-> Collector.Receive[CL_TEST];
	GreenHouseP.CtpSend 		-> CollectionSenderC;
	
	GreenHouseP.RadioControl 	-> ActiveMessageC;
	GreenHouseP.SerialControl 	-> SerialActiveMessageC;
	
	//GreenHouseP.SerialSend -> AMSenderC;
	GreenHouseP.SerialSend -> SerialActiveMessageC.AMSend[AM_GREENHOUSE];
	
	GreenHouseP.QueueCtp	-> QueueCtp;
	GreenHouseP.QueueSerial	-> QueueSerial;
	GreenHouseP.QueueTemp	-> QueueTemp;
	
	GreenHouseP.DiagMsg -> DiagMsgC;
}