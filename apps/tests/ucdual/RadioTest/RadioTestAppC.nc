
#include "RadioTest.h"
#include "RadioAssert.h"

configuration RadioTestAppC {}
implementation {
	components MainC, LedsC, DiagMsgC;
	components new TimerMilliC();

	App.Boot -> MainC.Boot;
	App.Leds -> LedsC;
	App.MilliTimer -> TimerMilliC;
	App.DiagMsg -> DiagMsgC;

	#if defined(RT_CHANNEL)
		components RadioTestChannelC as App, RadioStackC as Radio;

	#elif defined(RT_PAR)
		components RadioTestParallelC as App, RadioStackC as Radio;
	#elif defined(RT_XOR)
		components RadioTestXORC as App, RadioStackC as Radio;
	#elif defined(RT_MOPT)
		components RadioTestMoptC as App, RadioStackC as Radio;
	
	#elif defined(RT_AM)
		components RadioTestAMC as App;
		components new DirectAMSenderC(101) as DsC;
		components new AMReceiverC(101) as RsC;
		components Si443xActiveMessageC as MsgC;
		App.AMSend -> DsC;
		App.SplitControl -> MsgC;
		App.Packet -> MsgC;
		App.Receive -> RsC;
	#endif
	
#ifndef RT_AM	
	App.RadioState -> Radio;
	App.RadioSend -> Radio;
	App.RadioPacket -> Radio;
	App.RadioReceive -> Radio;	
#endif

}


