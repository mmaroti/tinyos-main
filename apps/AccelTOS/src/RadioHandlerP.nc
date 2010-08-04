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
		interface BufferedSend;
		interface LedHandler;
		interface SimpleFile as Disk;
		interface SplitControl as DiskCtrl;
		interface StdControl as MeterCtrl;
		interface StdControl as Sampling;
		interface Timer<TMilli> as WatchDog;
		interface Timer<TMilli> as ShortPeriod;
		interface Timer<TMilli> as Download;
		interface DiagMsg;
   }
}

implementation{
	
	bool booting = TRUE;
	
	// Tracks state of radio
	bool radioOn = FALSE;

	// Mode of the radio
	uint8_t mode  = CONTINUOUS;
	
	// Guards message
	bool sending = FALSE;
	message_t message;

	enum {
		SAMPLESIZE = 14,
		END_OF_DATA = 127, // FIXME Magic numbers, not updated automagically
	};
	
	enum {
		MAX_DATA_LEN = 510
	};
	
	// Guards sector[]
	bool diskBusy = FALSE;
	
	uint8_t sector[MAX_DATA_LEN];
	uint16_t head = 0;
	uint16_t tail = 0;
	
	// Schedules a sendSampleMsg task
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
			ASSERT(FAIL);
		}
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
	
	task void sendSamples() {
		
		error_t error = SUCCESS;
		
		if (diskBusy) {
			ASSERT(FAIL);
			return;
		}
		
		// FIXME This ugliness together with the locking should be pushed to SimpleFileP
		error = call Disk.seek(0);
		
		if (!error) {
			diskBusy = TRUE; // TODO Unlock !
		}
		else {
			ASSERT(FAIL);
		}
	}
	
	event void Disk.seekDone(error_t error){
		
		if (!error) {
			error = call Disk.read(sector, MAX_DATA_LEN);
		}
		
		if (error) {
			ASSERT(FAIL);
			diskBusy = FALSE;
		}
	}
	
	event void Disk.readDone(error_t error, uint16_t length) {
		
		if (!error) {
			dumpInt("Len", length);
			
			head = 0;
			tail = length;
			pending = TRUE;
			
			if (!call Download.isRunning())
				 call Download.startPeriodic(50);
		}
		else {
			ASSERT(FAIL);
			diskBusy = FALSE;
		}
	}
	
	task void sendSampleMsg();
	
	// FIXME Proper speed of downloading?
	event void Download.fired(){

		if (diskBusy&&pending) {
			post sendSampleMsg();
			pending = FALSE;
			dump("postDone");
		}
	}
	
	void sendNextMessage() {

		error_t error;
		
		dumpInt("head", head);
		// FIXME What if msg never reaches destination, ACK?
		error = call BufferedSend.send(sector+head, SAMPLESIZE);
		
		if (!error) {
			head += SAMPLESIZE;
			call LedHandler.sendingToggle();
			dump("buffSendOK");
		}
		else {
			dump("buffSendFail");
		}
		
		pending = TRUE;
	}
	
	void getNextSector() {
		
		error_t error;
		
		call BufferedSend.flush();
		call LedHandler.sendingToggle();
		dump("endSector");
		
		error = call Disk.read(sector, MAX_DATA_LEN);
		
		if      (error == EBUSY) {			
			pending = TRUE;
		}		
		else if (error == END_OF_DATA) {			
			call Download.stop();
			// TODO On updating the source check how sector is guarded!
			diskBusy = FALSE;
			dump("endDisc");
		}
		else if (error==SUCCESS) {
			;
		}
		else {
			ASSERT(FAIL);
		}
	}
	
	task void sendSampleMsg() {

		if (pending) {
			ASSERT(FAIL);
			return;
		}

		if ((tail-head)>=SAMPLESIZE) {

			sendNextMessage();
		}
		else {
			ASSERT(tail==head);
			getNextSector();
		}
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		
		CtrlMsg* pkt = (CtrlMsg*)payload;
		
		uint8_t cmd = pkt->cmd;
		
		error_t error = SUCCESS;
		
		call LedHandler.msgReceived();

		// FIXME What if diskBusy?
		// TODO New command cancel download?
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
			error = call Sampling.start();
		}
		else if (cmd == STOPSAMPLING) {
			error = call Sampling.stop();
		}
		else if (cmd == SENDSAMPLES) {
			error = post sendSamples();
			dump("cmdSendSamp");
		}
		else {
			ASSERT(FAIL);
		}
		
		ASSERT(error==SUCCESS);
		return msg;
	}

	event void AMControl.stopDone(error_t error) {

		if (error == SUCCESS) {
			radioOn = FALSE;
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
			call LedHandler.radioOn();
			
			broadcast();

			if (booting) {
				call WatchDog.startPeriodic(1000);
				error = call DiskCtrl.start();
				ASSERT(!error);
			}
			call ShortPeriod.startOneShot(50);
		}		

	}

	event void WatchDog.fired(){
		// S A -> start
		// S C -> start
		// A A -> stop
		// A C -> nothing, stay awake

		if ( !radioOn ) {
			error_t error = call AMControl.start();
			ASSERT(error==SUCCESS);
		}
		else {
			broadcast();
			if (mode == ALTERING)
				call ShortPeriod.startOneShot(50);
		}
	}
	
	event void ShortPeriod.fired() {

		if (radioOn && mode == ALTERING) {

			if (call AMControl.stop() != SUCCESS)
				call LedHandler.error();
		}
	}

	event void Disk.formatDone(error_t error) {		
		diskBusy = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}

	event void Disk.appendDone(error_t error) {
		diskBusy = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}

	event void DiskCtrl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}
}
