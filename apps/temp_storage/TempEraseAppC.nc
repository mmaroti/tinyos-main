#include "StorageVolumes.h"
configuration TempEraseAppC {
}
implementation {
	components MainC;
	components LedsC;
	components TempEraseC as App;
	components new LogStorageC(VOLUME_LOGTEST, TRUE);
		
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.LogWrite -> LogStorageC;
}
