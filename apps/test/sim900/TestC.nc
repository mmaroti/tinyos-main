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
	components new AMReceiverC(AM_GSMMSG);
	components new AMSenderC(AM_GSMMSG);
	
	
	TestP.Boot->MainC;
	TestP.RadioReceive->AMReceiverC;
	TestP.RadioSend->AMSenderC;
	TestP.GSMSend->GSMActiveMessageC.AMSend;
	TestP.GSMReceive->GSMActiveMessageC.Receive;
	TestP.GSMSplitControl->GSMActiveMessageC.SplitControl;
	TestP.RadioSplitControl->ActiveMessageC.SplitControl;
	TestP.Leds->LedsC;
	TestP.DiagMsg -> DiagMsgC;
}	