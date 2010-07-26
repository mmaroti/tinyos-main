#include "CtrlMsg.h"

configuration AccelAppC{
	
}
implementation{
	components MainC, AccelP, LedsC, SimpleFileC;
	components ActiveMessageC;
	components new AMReceiverC(AM_CTRLMSG) as AMRec;
	AccelP.Boot -> MainC;
	AccelP.Leds -> LedsC;
	AccelP.AMControl -> ActiveMessageC;
	AccelP.Receive -> AMRec;
	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
}