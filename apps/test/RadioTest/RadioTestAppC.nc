
#include "RadioTest.h"
#include "RadioAssert.h"

configuration RadioTestAppC {}
implementation {
	components MainC, RadioTestC as App, RadioStackC as Radio, LedsC;
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


