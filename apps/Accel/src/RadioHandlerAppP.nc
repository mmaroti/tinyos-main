#include "CtrlMsg.h"

configuration RadioHandlerAppP {
	
	provides {
		interface SplitControl;
	}

}

implementation{
	components RadioHandlerP;
	components ActiveMessageC;
	components new AMReceiverC(AM_CTRLMSG) as AMRec;
	components new TimerMilliC() as Timer1;
	components LedsC; // TODO A component handling the leds
	SplitControl = RadioHandlerP;
	RadioHandlerP.AMControl -> ActiveMessageC;
	RadioHandlerP.Receive -> AMRec;
	RadioHandlerP.TimerRadio -> Timer1;
	RadioHandlerP.Leds -> LedsC;
}