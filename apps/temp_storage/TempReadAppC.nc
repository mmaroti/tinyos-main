
#include <Timer.h>
#include "TempStorage.h"
#include "StorageVolumes.h"

configuration TempReadAppC {
}
implementation {
	components MainC;
	components LedsC;
	components TempReadC as App;
	components new TimerMilliC() as Timer0;
	components ActiveMessageC;
	components new AMSenderC(AM_BLINKTORADIOMSG);
	components new LogStorageC(VOLUME_LOGTEST, TRUE);
		
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Timer0 -> Timer0;
	App.Packet -> AMSenderC;
	App.AMSend -> AMSenderC;
	App.AMControl -> ActiveMessageC;
	App.LogRead-> LogStorageC;
	
}
