#include "GreenHouse.h"

configuration GreenHouseC{
}
implementation{

	components GreenHouseP, MainC;
	components LedsC;
	components new TimerMilliC();
	//Szenzor komponenes (hõmérséklet)
	components new SensirionSht11C();
	//Két komponens az adatgyûjtésben való részvételhez.*/
	components CollectionC as Collector;
	components new CollectionSenderC(CL_TEST);
	//Adatküldéshez használt komponenesek.
	components ActiveMessageC;
	components SerialActiveMessageC;
	/*	
		Csomagok kezelésére használt sor adattípusok.
		Tulajdonképpen bufferterületet biztosítanak, ami túl gyors küldési ráta esetén 
		a csomagok ideiglenes tárolására alkalmas.
	*/
	components new QueueC(message_t*, QUEUE_SIZE) as QueueCtp;
	components new QueueC(message_t*, QUEUE_SIZE) as QueueSerial;
	components new QueueC(message_t*, QUEUE_SIZE) as QueueTemp;
	
	//Konzolra való kiiratásra való komponens.
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
	
	GreenHouseP.SerialSend -> SerialActiveMessageC.AMSend[AM_GREENHOUSE];
	
	GreenHouseP.QueueCtp	-> QueueCtp;
	GreenHouseP.QueueSerial	-> QueueSerial;
	GreenHouseP.QueueTemp	-> QueueTemp;
	
	GreenHouseP.DiagMsg -> DiagMsgC;
}