generic module Led5M(bool inverting){
  provides 
  {
   	interface Init;
    interface Led5;
  }
  uses 
  {
    interface GeneralIO as Led0;
    interface GeneralIO as Led1;
    interface GeneralIO as Led2;
    interface GeneralIO as Led3;
    interface GeneralIO as Led4;
  }
}
implementation{
command error_t Init.init() {
    atomic {
      call Led0.makeOutput();
      call Led1.makeOutput();
      call Led2.makeOutput();
      call Led3.makeOutput();
      call Led4.makeOutput();
      if(!inverting)
      {
	      call Led0.clr();
	      call Led1.clr();
	      call Led2.clr();
	      call Led3.clr();
	      call Led4.clr();
	  }
	  else
	  {
	  	 call Led0.clr();
	      call Led1.set();
	      call Led2.set();
	      call Led3.set();
	      call Led4.set();
	  }
    }
    return SUCCESS;
  }

  async command void Led5.led0On() {
    (!inverting) ? call Led0.set() : call Led0.clr();
  }

  async command void Led5.led0Off() {
    (!inverting) ? call Led0.clr() : call Led0.set();
  }

  async command void Led5.led0Toggle() {
    call Led0.toggle();
    // this should be removed by dead code elimination when compiled for
    // the physical motes
  }

  async command void Led5.led1On() {
   (!inverting) ? call Led1.set() : call Led1.clr();
  }

  async command void Led5.led1Off() {
    (!inverting) ? call Led1.clr() : call Led1.set();
  }

  async command void Led5.led1Toggle() {
    call Led1.toggle();
  }

  async command void Led5.led2On() {
    (!inverting) ? call Led2.set() : call Led2.clr();
  }

  async command void Led5.led2Off() {
    (!inverting) ? call Led2.clr() : call Led2.set();
  }

  async command void Led5.led2Toggle() {
    call Led2.toggle();
  }
  
  async command void Led5.led3On() {
    (!inverting) ? call Led3.set() : call Led3.clr();
  }

  async command void Led5.led3Off() {
  (!inverting) ? call Led3.clr() : call Led3.set();
  }

  async command void Led5.led3Toggle() {
    call Led3.toggle();
  }
  
  async command void Led5.led4On() {
    (!inverting) ? call Led4.set() : call Led4.clr();
  }

  async command void Led5.led4Off() {
    (!inverting) ? call Led4.clr() : call Led4.set();
  }

  async command void Led5.led4Toggle() {
    call Led4.toggle();
  }

  async command uint8_t Led5.get() {
    uint8_t rval;
    atomic {
      rval = 0;
      if (call Led0.get()) {
        rval |= LEDS_LED0;
      }
      if (call Led1.get()) {
        rval |= LEDS_LED1;
      }
      if (call Led2.get()) {
        rval |= LEDS_LED2;
      }
      if (call Led3.get()) {
        rval |= LEDS_LED3;
      }
      if (call Led4.get()) {
        rval |= LEDS_LED4;
      }
    return rval;
  }
}

  async command void Led5.set(uint8_t val) {
    atomic {
      if (val & LEDS_LED0) {
        call Led5.led0On();
      }
      else {
        call Led5.led0Off();
      }
      if (val & LEDS_LED1) {
        call Led5.led1On();
      }
      else {
        call Led5.led1Off();
      }
      if (val & LEDS_LED2) {
        call Led5.led2On();
      }
      else {
        call Led5.led2Off();
      }
      if (val & LEDS_LED3) {
        call Led5.led3On();
      }
      else {
        call Led5.led3Off();
      }
      if (val & LEDS_LED4) {
        call Led5.led4On();
      }
      else {
        call Led5.led4Off();
      }
    }
  }
}