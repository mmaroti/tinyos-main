#include "gsm.h"
configuration TestC{
}
implementation{

	components MainC;
	components LedsC;
	components GSMActiveMessageC;
	components ActiveMessageC;
	components TestP;
	components DiagMsgC;
	components BusyWaitMicroC;
	
	
	TestP.Boot->MainC;
	TestP.AMSend->GSMActiveMessageC.AMSend;
	TestP.Receive->GSMActiveMessageC.Receive;
	TestP.GSMSplitControl->GSMActiveMessageC.SplitControl;
	TestP.RadioSplitControl->ActiveMessageC.SplitControl;
	TestP.Leds->LedsC;
	TestP.BusyWait->BusyWaitMicroC;
	TestP.DiagMsg -> DiagMsgC;
}	