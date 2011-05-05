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
#include "Bma180.h"
//#include <util/delay.h>

module OscilloscopeC @safe()
{
  uses {
    interface Boot;
    interface Read<uint8_t/*bma180_data_t*/>;
    interface SplitControl as SpiControl;
    interface Leds;
    interface AMSend;
    interface SplitControl as AMControl;
    interface Timer<TMilli>;
    interface DiagMsg;
  }
}
implementation
{

  /*event void I2CControl.startDone(error_t error) {
    if(error!=SUCCESS)
      call I2CControl.start();
    else
      call AMControl.start();
  }*/

  event void SpiControl.startDone(error_t error) {
    //call Leds.led2On();
    //call Read.read();
    call Timer.startPeriodic(1000);
    //call Leds.led3On();
  }

  event void AMControl.startDone(error_t error) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("Radio ok");
      call DiagMsg.send();
    } call Leds.led1On();
    call SpiControl.start();
  }

  event void Timer.fired() {
    call Read.read();
    call Leds.led3Toggle();
  }

  event void Boot.booted() {
    DDRF = _BV(PF0);
    PORTF = _BV(PF0);
   
    //call I2CControl.start();
    call AMControl.start();
    call Leds.led0On();
  }

  event void AMSend.sendDone(message_t* msg, error_t error) {}
  event void SpiControl.stopDone(error_t error) {}
  event void AMControl.stopDone(error_t error) {}
  event void Read.readDone(error_t result, uint8_t/*bma180_data_t*/ data) { 
    if(call DiagMsg.record()) { 
      call DiagMsg.str("Readdone ok"); 
      call DiagMsg.hex8(data); 
      call DiagMsg.send(); 
    }// _delay_ms(100);
    call Leds.led2Toggle();
   // call Read.read();
  }
}
