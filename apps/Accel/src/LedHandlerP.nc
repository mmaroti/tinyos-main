module LedHandlerP {
	
	provides interface LedHandler;
	
	uses interface Leds;

}

implementation{

	command void LedHandler.radioOn() {
		call Leds.led1On();
	}	

	command void LedHandler.radioOff() {
		call Leds.led1Off();
	}

	command void LedHandler.diskReady() {
		call Leds.led2On();
	}

	command void LedHandler.error() {
		call Leds.led0On();
	}
}