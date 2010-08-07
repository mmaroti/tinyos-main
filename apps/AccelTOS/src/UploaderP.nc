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

#include "Assert.h"

module UploaderP {

	uses {
		interface SimpleFile as Disk;
		interface BufferedSend;
		interface Timer<TMilli> as UploadTimer;
		interface LedHandler;
		interface DiagMsg;	
	}
	
	provides {
		interface Uploader;
	}
}

implementation {
	
	
	enum {
		SAMPLESIZE = 16, // FIXME Knows message size
		END_OF_DATA = 127, // FIXME Magic numbers, not updated automagically
	};
	
	enum {
		MAX_DATA_LEN = 508 // FIXME It should not know this implementation detail
	};
	
	// Guards sector[]
	bool diskBusy = FALSE;
	
	uint8_t sector[MAX_DATA_LEN];
	uint16_t head = 0;
	uint16_t tail = 0;
	
	// Schedules a sendSampleMsg task
	bool pending = FALSE;
	
	bool formatting = FALSE;  // FIXME Hideous
	/*
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
	*/
	event void Disk.seekDone(error_t error){
		
		if (!error) {
			error = call Disk.read(sector, MAX_DATA_LEN);
		}
		
		if (error) {
			ASSERT(FALSE);
			diskBusy = FALSE;
			signal Uploader.uploadDone(error);
		}
	}
	
	event void Disk.readDone(error_t error, uint16_t length) {
		
		if (!error) {
			//dumpInt("Len", length);
			
			head = 0;
			tail = length;
			pending = TRUE;
			
			if (!call UploadTimer.isRunning())
				 call UploadTimer.startPeriodic(50);
		}
		else {
			ASSERT(FALSE);
			diskBusy = FALSE;
			signal Uploader.uploadDone(error);
		}
	}
	
	task void sendSampleMsg();
	
	// FIXME Proper speed of uploading?
	event void UploadTimer.fired(){

		if (diskBusy&&pending) {
			post sendSampleMsg();
			pending = FALSE;
			//dump("postDone");
		}
	}
	
	void sendNextMessage() {

		error_t error;
		
		//dumpInt("head", head);
		// FIXME What if msg never reaches destination, ACK?
		error = call BufferedSend.send(sector+head, SAMPLESIZE);
		
		if (!error) {
			head += SAMPLESIZE;
			call LedHandler.sendingToggle();
			//dump("buffSendOK");
		}
		else {
			//dump("buffSendFail");
		}
		
		pending = TRUE;
	}
	
	void getNextSector() {
		
		error_t error;
		
		call BufferedSend.flush();
		call LedHandler.sendingToggle();
		//dump("endSector");
		
		error = call Disk.read(sector, MAX_DATA_LEN);
		
		if      (error == EBUSY) {			
			pending = TRUE;
		}		
		else if (error == END_OF_DATA) {			
			call UploadTimer.stop(); // FIXME Never shut down in case of an error
			// TODO On updating the source check how sector is guarded!
			//dump("endDisc");
			diskBusy = FALSE;
			signal Uploader.uploadDone(SUCCESS);
		}
		else if (error==SUCCESS) {
			;
		}
		else {
			ASSERT(FALSE);
		}
	}
	
	task void sendSampleMsg() {

		if (pending) {
			ASSERT(FALSE);
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

	command error_t Uploader.format() {
		
		error_t error = call Disk.format();
		if (!error)
			formatting = TRUE;
		return error;
	}

	event void Disk.formatDone(error_t error) {

		diskBusy = FALSE;		
		call LedHandler.diskReady();		
		
		ASSERT(!error);

		if (formatting) {
			formatting = FALSE;
			signal Uploader.formatDone(error);
		}
	}

	event void Disk.appendDone(error_t error) {

		diskBusy = FALSE;
		call LedHandler.diskReady();
		if (error)
			call LedHandler.error();
	}

	command error_t Uploader.upload(){
		
		error_t error = SUCCESS;
		if (diskBusy) {
			ASSERT(FALSE);
			return FAIL;
		}
		// FIXME This ugliness together with the locking should be pushed to SimpleFileP
		error = call Disk.seek(0);
		if (!error) {
			diskBusy = TRUE; // TODO Unlock !
		}
		else {
			ASSERT(FALSE);
		}
		return error;
	}
}
