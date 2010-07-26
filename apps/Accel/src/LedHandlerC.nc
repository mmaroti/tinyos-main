
configuration LedHandlerC {
	
	provides interface LedHandler;

}

implementation {
	
	components LedsC, LedHandlerP;

	LedHandler = LedHandlerP;	
	LedHandlerP.Leds -> LedsC;

}