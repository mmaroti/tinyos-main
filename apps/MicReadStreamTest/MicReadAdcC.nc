/** Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Zoltan Kincses
*/

#include "MicReadAdc.h"

module MicReadAdcC
{
  uses {
    interface Boot;
    interface Leds;
    interface Resource as AdcResource;
    interface Atm128AdcSingle;
    interface MicaBusAdc as MicAdcChannel;
    interface SplitControl as Microphone;
    interface Alarm<TMilli, uint32_t>;
  }
}
implementation {
  
	bool granted, started; // Status of start request

	void detect() {
		call Atm128AdcSingle.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF, FALSE,ATM128_ADC_PRESCALE_16);
	}
	
	task void stopMicrophone() {
    /* We're done. Power down the microphone, release the ADC and hand
       control back to SynchronizerC */
    	atomic granted = FALSE; // Note that we're no longer the ADC owner
    	call AdcResource.release();
    	call Microphone.stop();
	}
	
	event void Microphone.stopDone(error_t error) {
    	atomic started = error != SUCCESS;
	}
	
	event void Boot.booted()  {
		atomic granted = started = FALSE;
			
		call Alarm.start(1024);
		call Microphone.start();
		call AdcResource.request();
		
		call Leds.led0On();
	}

	event void AdcResource.granted() {
    	atomic granted = TRUE;  // Note when ADC granted
	}

	event void Microphone.startDone(error_t error) {
		atomic started = error == SUCCESS; // Note if microphone started
	}

	async event void Alarm.fired() {
		// It's time to detect a loud sound. If we didn't get the ADC or
		// turn on the microphone in time, report a failed event detection.
		atomic
		if (granted && started) {
			call Leds.led1On();
			call Leds.led2Off();
			detect();
		} else {
			call Leds.led2On();
			call Alarm.start(1024);
		}
	}

	async event void Atm128AdcSingle.dataReady(uint16_t data, bool precise) {
    /* If we're the current ADC owner: check ADC completion events to
       see if the microphone is above the threshold  */
		atomic
		if (precise){
	    	post stopMicrophone();
	  	} else {
	  		detect();
	  	}
	}
}
