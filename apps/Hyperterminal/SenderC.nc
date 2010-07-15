#include "DataMsg.h"

configuration SenderC {
}
implementation {
	components MainC;
	components LedsC;
	components Atm128Uart0C;
	components CounterMicro32C;
	components ActiveMessageC;
	components new AMReceiverC(AM_DATAMSG);
	components new AMSenderC(AM_DATAMSG);
	components new TimerMilliC() as Timer;
	components MicaBusC;
	components SenderP as App;
	components DiagMsgC;
	
	App.Boot -> MainC;
	App.Leds -> LedsC;
	App.Receive -> AMReceiverC;
	App.AMSend -> AMSenderC;
	App. Timer -> Timer;
	App.SplitControl -> ActiveMessageC;
	App.UartByte -> Atm128Uart0C.UartByte;
	App.UartStream -> Atm128Uart0C.UartStream;
	App.StdControl -> Atm128Uart0C.StdControl;
	App.RTS->MicaBusC.PW2;
	App.DTR->MicaBusC.PW3;
	App.CTS->MicaBusC.Int3_Interrupt;
	App.DSR->MicaBusC.Int0_Interrupt;
	Atm128Uart0C.Counter -> CounterMicro32C.Counter;
	App.DiagMsg->DiagMsgC;
	

	
}


