
#include "RadioTest.h"
#include "RadioAssert.h"

configuration RadioTestAppC {}
implementation {
	components MainC, RadioStackC as Radio, LedsC;
	#ifdef RT_PARALLEL 
		components RadioTestParallelC as App;
	#else
		components RadioTestXORC as App;
	#endif
	
	components new TimerMilliC();
	components DiagMsgC;
	
	App.Boot -> MainC.Boot;
	App.Leds -> LedsC;
	App.MilliTimer -> TimerMilliC;
	App.DiagMsg -> DiagMsgC;

	App.RadioState -> Radio;
	App.RadioSend -> Radio;
	App.RadioPacket -> Radio;
	App.RadioReceive -> Radio;
}


