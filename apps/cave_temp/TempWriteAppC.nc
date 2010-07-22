
#include <Timer.h>
#include "TempStorage.h"
#include "StorageVolumes.h"

configuration TempWriteAppC {
}
implementation {
	components MainC, TimeSyncC;

	MainC.SoftwareInit -> TimeSyncC;
  	TimeSyncC.Boot -> MainC;
	
	components LedsC;
	components TempWriteC as App;
	components new TimerMilliC() as Timer0;
	components new TemperatureC() as Sensor;
	components new LogStorageC(VOLUME_LOGTEST, TRUE);
		
	components ActiveMessageC;
	  App.RadioControl -> ActiveMessageC;
	  App.Receive -> ActiveMessageC.Receive[AM_CONTROLMSG];
	  App.AMSend -> ActiveMessageC.AMSend[AM_BLINKTORADIOMSG];
	  App.Packet -> ActiveMessageC;


	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Timer0 -> Timer0;
	App.Read -> Sensor;
	App.LogWrite -> LogStorageC;
	App.LogRead-> LogStorageC;
	App.GlobalTime -> TimeSyncC;
	App.TimeSyncNotify -> TimeSyncC;
 	App.TimeSyncInfo -> TimeSyncC;
	
}
