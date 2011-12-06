configuration GSMActiveMessageC{
	provides
	{
		interface SplitControl;
		interface AMSend;
		interface Receive;

	}
}
implementation{
	
	components GSMActiveMessageP,GSMDriverP,MainC;
	components new TimerMilliC() as Timer0, new TimerMilliC() as Timer1, new TimerMilliC() as Timer3;
	components MicaBusC;
 	
 	components Atm128FCUart0C as UART, CounterMicro32C;
	components HplAtm128GeneralIOC as GPIO;
	components new Atm128GpioInterruptC() as CTSINT;
 	components HplAtm128InterruptC as INT;
	
	components DiagMsgC;
	
	
	SplitControl=GSMActiveMessageP.SplitControl;
	AMSend=GSMActiveMessageP.AMSend;
	Receive=GSMActiveMessageP.Receive;
	
	UART.Counter -> CounterMicro32C;
	CTSINT.Atm128Interrupt -> INT.Int2;
  	UART.CTSInt -> CTSINT.Interrupt;
  	UART.CTSPin -> MicaBusC.Int2;
  	
  	GSMDriverP.ON->MicaBusC.PW3;
	GSMDriverP.UartStream->UART;
	GSMDriverP.StdControl->UART;
	GSMDriverP.TimerPower->Timer0;
	GSMDriverP.TimerAnswer->Timer1;
	GSMDriverP.TimerDelay->Timer3;
	GSMActiveMessageP.GsmControl->GSMDriverP.GsmControl; 
	
	
}