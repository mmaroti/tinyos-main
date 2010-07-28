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
		interface AMSend;
		interface LedHandler;
		interface SimpleFile as Disk;
		interface Timer<TMilli> as WatchDog;
		interface Timer<TMilli> as ShortPeriod;
		interface LocalTime<TMilli> as LocTime;
   }
   
   provides {
		interface StdControl;
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
	uint8_t mode  = ALTERING;
	
	// Guards report
	bool sending = FALSE;
	message_t report;
	
	// Guards dataPkt
	bool writing = FALSE;
	struct {
		uint16_t node_id;
		uint32_t local_time;
	} dataPkt;
	// TODO Use nx_unit ?

	error_t broadcast() {

		error_t error;
		
		if (sending) {
			error =  EBUSY;
		}
		else {
			// TODO Explain why
    		// Note that we could have avoided using the Packet interface, as it's 
    		// getPayload command is repeated within AMSend.
    		ReportMsg* pkt = (ReportMsg*)(call AMSend.getPayload(&report, NULL));
    		pkt->id = TOS_NODE_ID;
    		pkt->mode = mode;
 			error = call AMSend.send(AM_BROADCAST_ADDR, &report, sizeof(ReportMsg));
    		if (error == SUCCESS) {
      			sending = TRUE;
    		}
    	}
    	return error;
	}
	
	event void AMSend.sendDone(message_t *msg, error_t error){
		sending = FALSE;
		// FIXME Resend if failed?
	}
	

	command error_t StdControl.stop(){
		// TODO Finish impl of stop
		return FAIL;
	}

	// FIXME StdControl is not an appropriate interface
	command error_t StdControl.start(){
		// FIXME Finish impl of start

		return call AMControl.start();
		
	}
	
	task void appendPacket() {

		error_t error = SUCCESS;
		
		if (writing)
			return;
		
		dataPkt.node_id = TOS_NODE_ID;
		dataPkt.local_time = call LocTime.get();
		
		error = call Disk.append((uint8_t *) &dataPkt, sizeof(dataPkt));
		
		if (!error) {
			writing = TRUE;
		}
		else {
			call LedHandler.error();
		}
	}
	
	task void sendFirstPkt() {
		
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
				if (!writing) {
					error = call Disk.format();
					if (!error)
						writing = TRUE;
				}
			}
			else if (cmd == APPENDPKT) {
				error = post appendPacket();
			}
			else if (cmd == SENDFIRST) {
				error = post sendFirstPkt();
			}
			// FIXME What if unknown mode received? Or msg corrupted?
			
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
		
		if (error == SUCCESS) {
			
			state = AWAKE;
			call LedHandler.radioOn();
			
			broadcast();
			
			call ShortPeriod.startOneShot(50);
			if (! call WatchDog.isRunning()) {
				call WatchDog.startPeriodic(1000);
			}
		}		
		else
			call LedHandler.error();
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
				call ShortPeriod.startOneShot(200);
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
		writing = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}

	event void Disk.seekDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void Disk.appendDone(error_t error){
		writing = FALSE;		
		call LedHandler.diskReady();		
		if (error)
			call LedHandler.error();
	}

	event void Disk.readDone(error_t error, uint16_t length){
		// TODO Auto-generated method stub
	}
}