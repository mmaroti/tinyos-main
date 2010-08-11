/** Copyright (c) 2010, University of Szeged
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
* Author: Ali Baharev
*/

#include "CtrlMsg.h"
#include "ReportMsg.h"
#include "Assert.h"

module RadioHandlerP{

   uses {
   		interface Boot;
		interface SplitControl as AMControl;
		interface Receive;
		interface AMSend as AMReportMsg;
		interface LedHandler;
		interface SplitControl as DiskCtrl;
		interface StdControl as MeterCtrl;
		interface SplitControl as Sampling;
		interface Uploader;
		interface Timer<TMilli> as WatchDog;
		interface Timer<TMilli> as ShortPeriod;
		interface DiagMsg;
   }
}

implementation{
	
	enum {
		SHORTPERIOD = 50,
		WATCHDOG    = 61440
	};
	
	bool booting = TRUE;
	
	// Tracks state of radio
	bool radioOn = FALSE;

	// Mode of the radio
	uint8_t mode  = CONTINUOUS;
	
	// Guards message
	bool sending = FALSE;
	message_t message;
	
	// Tracks the state of the disc
	bool diskBusy = FALSE;

	void dump(char* msg) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.send();
		}	
	}
	
	event void Boot.booted(){
		
		error_t error = call AMControl.start();
		if (error)
			call LedHandler.error();
	}
	
	event void DiskCtrl.startDone(error_t error){
		ASSERT(!error);

		call LedHandler.diskReady();
		
		error = call MeterCtrl.start();
		if (!error) {
			booting = FALSE;
		}
		else {
			ASSERT(FALSE);
		}
	}
	
	event void DiskCtrl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

	error_t broadcast() {

		error_t error;
		
		if (sending) {
			error =  EBUSY;
		}
		else {
    		ReportMsg* pkt = (ReportMsg*)(call AMReportMsg.getPayload(&message, sizeof(ReportMsg)));
			ASSERT(pkt != NULL);

    		pkt->id = TOS_NODE_ID;
    		pkt->mode = mode;
 			error = call AMReportMsg.send(AM_BROADCAST_ADDR, &message, sizeof(ReportMsg));
    		if (error == SUCCESS) {
      			sending = TRUE;
    		}
    	}
    	return error;
	}
	
	event void AMReportMsg.sendDone(message_t *msg, error_t error){
		sending = FALSE;
		// TODO Resend if failed?
	}
	
	task void turnRadioOff() {

		error_t error = call AMControl.stop(); // FIXME What if already turned off
		if (!error) {
			radioOn = FALSE;
		}		
		else {
			call LedHandler.error();
		}
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		
		CtrlMsg* pkt = (CtrlMsg*)payload;
		
		uint8_t cmd = pkt->cmd;
		
		error_t error = SUCCESS;
		
		call LedHandler.msgReceived();

		// TODO New command to cancel upload?
		if (cmd == STOPSAMPLING) {
			error = call Sampling.stop();
		}
		else if (cmd == RADIOOFF) {
			call WatchDog.stop();
			mode = RADIOOFF;
			post turnRadioOff();
		}
		else if (diskBusy) {
			ASSERT(FALSE); // FIXME Just for dbg
		}
		else if (cmd == ALTERING)   {
			mode = ALTERING;
			call ShortPeriod.startOneShot(SHORTPERIOD);
		}
		else if (cmd == CONTINUOUS) {
			mode = CONTINUOUS;
		}
		else if (cmd == FORMAT) {
			error = call Uploader.format();
			if (!error)
				diskBusy = TRUE;
		}
		else if (cmd == APPENDPKT) {
			;
		}
		else if (cmd == SENDFIRST) {
			;
		}
		else if (cmd == STARTSAMPLING) {
			error = call Sampling.start();
			if (!error)
				diskBusy = TRUE;
		}
		else if (cmd == SENDSAMPLES) {
			error = call Uploader.upload();
			dump("cmdSendSamp");
		}
		else {
			ASSERT(FALSE);
		}
		
		ASSERT(error==SUCCESS);
		return msg;
	}

	event void AMControl.stopDone(error_t error) {

		if (error == SUCCESS) {
			ASSERT(!radioOn);
			call LedHandler.radioOff();
		}		
		else {
			call LedHandler.error();
		}
	}

	event void AMControl.startDone(error_t error) {
		
		ASSERT(error == SUCCESS);
		
		if (error == SUCCESS) {
			
			radioOn = TRUE;
			//call LedHandler.radioOn();
			
			broadcast();

			if (booting) {
				call WatchDog.startPeriodic(WATCHDOG);
				error = call DiskCtrl.start();
				ASSERT(!error);
			}
			call ShortPeriod.startOneShot(SHORTPERIOD);
		}		
	}

	event void WatchDog.fired(){
		// S A -> start
		// S C -> start
		// A A -> stop
		// A C -> nothing, stay awake
		
		error_t error;
		
		if (mode == RADIOOFF) {
			call WatchDog.stop();
		}
		else if (!radioOn) {
			error = call AMControl.start();
			ASSERT(!error);
		}
		else {
			broadcast();
			if (mode == ALTERING)
				call ShortPeriod.startOneShot(SHORTPERIOD);
		}
	}
	
	event void ShortPeriod.fired() {

		if (radioOn && mode == ALTERING) {

			if (call AMControl.stop() != SUCCESS) {
				call LedHandler.error();
			}
			else {
				radioOn = FALSE;
			}
		}
	}

	event void Uploader.formatDone(error_t error){
		ASSERT(!error);
		diskBusy = FALSE;
	}

	event void Uploader.uploadDone(error_t error){
		ASSERT(!error);
		diskBusy = FALSE;
	}

	event void Sampling.stopDone(error_t error){
		diskBusy = FALSE;
	}

	event void Sampling.startDone(error_t error){
		// FIXME This should be ignored plus sampling may already be in progress... :(
	}
}
