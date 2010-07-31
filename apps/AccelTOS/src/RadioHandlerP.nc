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

module RadioHandlerP{

   uses {
		interface SplitControl as AMControl;
		interface Receive;
		interface AMSend as AMReportMsg;
		interface BufferedSend;
		interface LedHandler;
		interface SimpleFile as Disk;
		interface Meter;
		interface Timer<TMilli> as WatchDog;
		interface Timer<TMilli> as ShortPeriod;
		interface Timer<TMilli> as Download;
		interface DiagMsg;
   }
   
   provides {
		interface SplitControl;
   	}
}

implementation{
	
	enum {
		SLEEP,
		AWAKE,
	};
	
	// Tracks state of radio
	bool state = SLEEP;

	// Mode of the radio
	uint8_t mode  = CONTINUOUS;
	
	// Guards message
	bool sending = FALSE;
	message_t message;
	
	// Guards sector
	bool diskBusy = FALSE;
	
	enum {
		SAMPLESIZE = 14,
		END_OF_DATA = 127, // FIXME Magic numbers
	};
	
	enum {
		MAX_DATA_LEN = 510
	};

	//uint16_t offset =  0;
	//uint16_t dataLen = 0;
	uint8_t sector[MAX_DATA_LEN]; // Guarded by diskBusy
	uint16_t head = 0;
	uint16_t tail = 0;
	bool pending = FALSE;

	void dump(char* msg) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.send();
		}	
	}
	
	void dumpInt(char* msg, uint16_t i) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.int16(i);
			call DiagMsg.send();
		}		
	}

	error_t broadcast() {

		error_t error;
		
		if (sending) {
			error =  EBUSY;
		}
		else {
			// TODO Explain why
    		// Note that we could have avoided using the Packet interface, as it's 
    		// getPayload command is repeated within AMSend.
    		ReportMsg* pkt = (ReportMsg*)(call AMReportMsg.getPayload(&message, sizeof(ReportMsg*)));
			if( pkt == NULL )
				call LedHandler.error();

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
		// FIXME Resend if failed?
	}
	
	task void sendSamples() {
		
		error_t error = SUCCESS;
		
		if (diskBusy) {
			call LedHandler.error(); // FIXME Just for dbg
			return;
		}
		
		error = call Disk.seek(0);
		
		if (!error) {
			diskBusy = TRUE; // TODO Unlock !
		}
		else {
			call LedHandler.error();
		}
	}
	
	event void Disk.seekDone(error_t error){
		
		if (!error) {
			error = call Disk.read(sector, MAX_DATA_LEN);
		}
		
		if (error) {
			call LedHandler.error();
			diskBusy = FALSE;
		}
	}
	
	task void sendSampleMsg() {
		
		error_t error;
		pending = FALSE;
		
		//if (offset <= dataLen-SAMPLESIZE) {
		if ((tail-head)>=SAMPLESIZE) {
			dumpInt("head", head);
			dumpInt("tail", tail);			
			//error = call BufferedSend.send(sector+offset, SAMPLESIZE);
			error = call BufferedSend.send(&(sector[head]), 14);
			
			if (!error) {
				//offset += SAMPLESIZE;
				head = head + SAMPLESIZE;
				call LedHandler.sendingToggle();
				dump("buffSendOK");
			}
			else {
				call LedHandler.errorToggle();
				dump("buffSendFail");
			}
			
			pending = TRUE;
//			error = post sendSampleMsg();
//			if (error)
//				dump("post1Failed");
		}
		else {
			
			call BufferedSend.flush();
			call LedHandler.sendingToggle();
			dump("endSector");
			error = call Disk.read(sector, MAX_DATA_LEN);
			
			if      (error == EBUSY) {
				pending = TRUE;
//				error = post sendSampleMsg();
//				if (error)
//					dump("post2Failed");
			}
			if (error == END_OF_DATA) {
				call Download.stop();
				diskBusy = FALSE;
				dump("endDisc");
			}			
		}
	}
	
	event void Download.fired(){

		if (diskBusy&&pending) {
			error_t error = post sendSampleMsg();
			if (error)
				dump("postTFailed");
		}
	}
	
	event void Disk.readDone(error_t error, uint16_t length) {
		
		if ((!error) && (length>0)) {
			dumpInt("Len", length);
			head = 0;
			tail = length;
			pending = FALSE;
			call Download.startPeriodic(50);
			error = post sendSampleMsg();
			if (error)
				dump("postRFailed");
		}
		else {
			dumpInt("readFail", length);
		}
		
		if (error) {

			call LedHandler.error();
			diskBusy = FALSE; // FIXME How is sector guarded?
		}
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		
		if (len == sizeof(CtrlMsg)) {  // TODO Enough?
		
			CtrlMsg* pkt = (CtrlMsg*)payload;
			
			uint8_t cmd = pkt->cmd;
			
			error_t error = SUCCESS;
			
			call LedHandler.msgReceived();

			if      (cmd == ALTERING)   {
				mode = ALTERING;
				call ShortPeriod.startOneShot(50);
			}
			else if (cmd == CONTINUOUS) {
				mode = CONTINUOUS;
			}
			else if (cmd == FORMAT) {
				if (!diskBusy) {
					error = call Disk.format();
					if (!error)
						diskBusy = TRUE;
				}
			}
			else if (cmd == APPENDPKT) {
				//error = post appendPacket();
			}
			else if (cmd == SENDFIRST) {
				//error = post sendFirstPkt();
			}
			else if (cmd == STARTSAMPLING) {
				error = call Meter.startRecording();
			}
			else if (cmd == STOPSAMPLING) {
				error = call Meter.stopRecording();
			}
			else if (cmd == SENDSAMPLES) {
				error = post sendSamples();
				dump("cmdSendSamp");
			}
			// FIXME What if unknown mode received? Or msg corrupted?
			else {
				call LedHandler.error();
			}
			
			if (error) {
				call LedHandler.error();
			}
		}

		return msg;
	}

	event void AMControl.stopDone(error_t error) {

		if (error == SUCCESS) {
			state = SLEEP;
			call LedHandler.radioOff();
		}		
		else
			call LedHandler.error();
	}

	event void AMControl.startDone(error_t error) {
		
		// FIXME Only DiadMsg could have started the radio
		if ((error == SUCCESS) || (error == EALREADY)) {
			
			state = AWAKE;
			call LedHandler.radioOn();
			
			broadcast();

			if (! call WatchDog.isRunning()) {
				call WatchDog.startPeriodic(1000);
				// FIXME Would signal even if already started
				signal SplitControl.startDone(SUCCESS); // FIXME Sure?
			}
			call ShortPeriod.startOneShot(50);
		}		
		else {
			call LedHandler.error();
			signal SplitControl.startDone(error);
		}
	}

	command error_t SplitControl.stop(){
		// TODO Auto-generated method stub
		call LedHandler.error();
		return FAIL;
	}

	command error_t SplitControl.start(){
		// FIXME Finish impl of start

		return call AMControl.start();
	}

	event void WatchDog.fired(){

		error_t error = SUCCESS;

		// S A -> start
		// S C -> start
		// A A -> stop
		// A C -> nothing, stay awake

		if      (state == SLEEP) {
			error = call AMControl.start();
		}
		else {
			broadcast();
			if (mode == ALTERING)
				call ShortPeriod.startOneShot(50); // FIXME What if already running?
		}

		if (error != SUCCESS)
			call LedHandler.error();
	}
	
	event void ShortPeriod.fired() {

		if (state == AWAKE && mode == ALTERING) {

			if (call AMControl.stop() != SUCCESS)
				call LedHandler.error();
		}
	}

	event void Disk.formatDone(error_t error){		
		diskBusy = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}

	event void Disk.appendDone(error_t error){
		diskBusy = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}


}
