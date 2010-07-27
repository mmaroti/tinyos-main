#include "CtrlMsg.h"

configuration RadioHandlerAppP {
	
	provides {
		interface StdControl;
	}

}

implementation{
	components RadioHandlerP;
	components ActiveMessageC;
	components new AMReceiverC(AM_CTRLMSG) as AMRec;
	components new AMSenderC(AM_CTRLMSG) as AMSender;
	components new TimerMilliC() as Timer1;
	components LedHandlerC;
	StdControl = RadioHandlerP;
	RadioHandlerP.AMControl -> ActiveMessageC;
	RadioHandlerP.Receive -> AMRec;
	RadioHandlerP.AMSend -> AMSender;
	RadioHandlerP.TimerRadio -> Timer1;
	RadioHandlerP.LedHandler -> LedHandlerC;
}