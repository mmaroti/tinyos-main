module NullPCIntC
{
	uses interface Alarm<T62khz, uint32_t>;
  uses interface Boot;
	uses interface GpioPCInterrupt as PCINT;
	uses interface Leds;
	
}
implementation
{
  event void Boot.booted() {
	//call pb1.clr();
	call PCINT.enable();
	SCCR0 = (1 << SCEN);
	call Alarm.start(0x0100);
/*if (!call PCINT.get())
	call Leds.led1On();
else
	call Leds.led1Off();
call Leds.led2On();*/
    // Do nothing.
  }

task void ledon() {
	call Leds.led0On();
}

task void ledoff() {
	call Leds.led0Off();
}

async event void PCINT.fired(bool toHigh) {
	if(toHigh) {
		post ledon();
	}
	else {
		post ledoff();
	}
}

async event void Alarm.fired() {
		call Alarm.stop();
		//ison = ~ison;
		//ison?(call Leds.led3On()):(call Leds.led3Off());
		call Alarm.start(0xFF00);
	}
}

