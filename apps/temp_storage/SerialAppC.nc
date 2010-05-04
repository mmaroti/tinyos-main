
#include <Timer.h>
#include "TempStorage.h"

configuration SerialAppC {
}
implementation {
	components MainC;
	components LedsC;
	components SerialC as App;
	components ActiveMessageC;
	components new AMReceiverC(AM_BLINKTORADIOMSG);
	components SerialActiveMessageC;
	components new SerialAMSenderC(AM_BLINKTORADIOMSG);
	components new SerialAMReceiverC(AM_BLINKTORADIOMSG);
			
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.AMControl -> ActiveMessageC;
	App.Receive -> AMReceiverC;
	App.SerialAMSend -> SerialAMSenderC;
	App.SerialReceive -> SerialAMReceiverC;
	App.SerialControl -> SerialActiveMessageC;
	
}


