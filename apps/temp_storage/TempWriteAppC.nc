
#include <Timer.h>
#include "TempStorage.h"
#include "StorageVolumes.h"

configuration TempWriteAppC {
}
implementation {
	components MainC;
	components LedsC;
	components TempWriteC as App;
	components new TimerMilliC() as Timer0;
	components new TemperatureC() as Sensor;
	components new LogStorageC(VOLUME_LOGTEST, TRUE);
		
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Timer0 -> Timer0;
	App.Read -> Sensor;
	App.LogWrite -> LogStorageC;
	
}
