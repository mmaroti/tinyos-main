#include "gsm.h"
configuration TestC{
}
implementation{

	components MainC;
	components LedsC;
	components GSMActiveMessageC;
	components ActiveMessageC;
	components TestP;
	//components new AMReceiverC(AM_GSMMSG);
	components DiagMsgC;
	components BusyWaitMicroC;
	
	
	TestP.Boot->MainC;
	TestP.AMSend->GSMActiveMessageC.AMSend;
	TestP.GSMSplitControl->GSMActiveMessageC.SplitControl;
	TestP.RadioSplitControl->ActiveMessageC.SplitControl;
	//TestP.Receive->AMReceiverC;
	TestP.Leds->LedsC;
	TestP.BusyWait->BusyWaitMicroC;
	TestP.DiagMsg -> DiagMsgC;
}	