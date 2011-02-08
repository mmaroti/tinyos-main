/*
 * Copyright (c) 2006 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */

/**
 * Oscilloscope demo application. See README.txt file in this directory.
 *
 * @author David Gay
 */

module OscilloscopeC @safe()
{
  uses {
    interface Boot;
    interface Read<uint16_t>;
    interface SplitControl as I2CControl;
		interface Leds;
		interface AMSend;
  }
}
implementation
{

	
	event void I2CControl.startDone(error_t error) {
    call Read.read();
  }

  event void Boot.booted() {
		PORTD = _BV(PD0) | _BV(PD1);
		DDRF = _BV(PF2);
		PORTF = _BV(PF2);
    call I2CControl.start();
    
  }

  event void AMSend.sendDone(message_t* msg, error_t error) {}
  event void I2CControl.stopDone(error_t error) {}
  event void Read.readDone(error_t result, uint16_t data) {}
}
