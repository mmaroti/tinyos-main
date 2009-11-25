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
