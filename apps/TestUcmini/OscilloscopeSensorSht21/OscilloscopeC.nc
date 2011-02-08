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
#include "Timer.h"
#include "Oscilloscope.h"

module OscilloscopeC @safe()
{
  uses {
    interface Boot;
    interface SplitControl as RadioControl;
    interface AMSend;
    interface Receive;
    interface Timer<TMilli>;
    interface Read<uint16_t>;
    interface Leds;
interface DiagMsg;

    interface SplitControl as I2CControl;
  }
}
implementation
{
  message_t sendBuf;


  // Use LEDs to report various status issues.
  void report_problem() { /*call Leds.led0Toggle();*/ }
  void report_sent() { /*call Leds.led1Toggle();*/ }
  void report_received() { /*call Leds.led2Toggle();*/ }
	event void I2CControl.startDone(error_t error) {
		call Timer.startOneShot(3000);
  }
  event void I2CControl.stopDone(error_t error) {}

  event void Boot.booted() {
    if (call I2CControl.start() != SUCCESS)
      report_problem();
    
  }


  event void RadioControl.startDone(error_t error) {
    call Timer.startPeriodic(3000);
  }

  event void RadioControl.stopDone(error_t error) {
  }

  event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
    return msg;
  }

  event void Timer.fired() {
    if (call Read.read() != SUCCESS)
      report_problem();
  }

  event void AMSend.sendDone(message_t* msg, error_t error) {

  }

  event void Read.readDone(error_t result, uint16_t data) {

  }
}
