/*
* Copyright (c) 2010, University of Szeged
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
* Author: Zsolt Szabo
*/

module I2CTestP {
	provides interface Read<uint16_t>;
	uses {
		interface SplitControl;
		interface I2CWrite;
		interface I2CRead as ReadAck;
		interface I2CRead as ReadNack;
		interface Timer<TMilli>;
	}
}
implementation {
	enum {
		IDLE = 0,
		ADDRESS,
		DATA,
		RESTART,
		MSB,
		LSB,
	} ;

	uint16_t result;
	uint8_t state = IDLE;

	command error_t Read.read() {
		call SplitControl.start();
		return SUCCESS;
	}

	event void SplitControl.startDone(error_t err) {
		if(state == IDLE) {
			atomic state = ADDRESS;
			call I2CWrite.write(128); //write address
		} else if (state == RESTART) {
			call I2CWrite.write(129); //read address
		}
	}

	event void I2CWrite.writeDone(uint8_t status) {
		if(state == ADDRESS) {
			atomic state = DATA;
			call I2CWrite.write(0xF3); //trigger temperature measurement
		} else if(state == DATA) {
			atomic state = RESTART;
			call Timer.startOneShot(100);
		} else if(state == RESTART) {
			atomic state = MSB;
			call ReadAck.read();
		}
	}

	event void ReadAck.readDone(uint8_t status, uint8_t value) {
		if(state == MSB) {
			atomic state = LSB;
			result = (value << 8);
			call ReadNack.read();
		}
	}

	event void ReadNack.readDone(uint8_t status, uint8_t value) {
		if(state == LSB) {
			atomic state = IDLE;
			result |= value;
			call SplitControl.stop();
		}
	}

	event void SplitControl.stopDone(error_t err) {
		signal Read.readDone(SUCCESS, result);
	}

	event void Timer.fired() {
		if(state == RESTART) {
			call SplitControl.start();
		}
	}

	default event void Read.readDone(error_t err, uint16_t val) {}
}
