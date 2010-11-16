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


module I2CTestDriverP {
  provides interface SplitControl;
	provides interface I2CWrite;
	provides interface I2CRead as ReadAck;
	provides interface I2CRead as ReadNack;
	provides interface Init;
}
implementation {
	
	enum {
		IDLE = 0,
		STARTCOND,
		STOPCOND,
		WRITEBYTE,
		R_ACK,
		R_NACK,
	}	;
	
	uint8_t state = IDLE;

	void TWI_Init();
	void TWI_Start();
	uint8_t TWI_Status();
	void TWI_Read(bool);
	void TWI_Write(uint8_t);
	void TWI_Stop();

	void TWI_Init() {
		atomic {
			TWSR = 0;
			TWSR = 48;
			TWCR = (1 << TWEN);//4; //TWEN
		}//atomic
	}//TWI_Init

	void TWI_Start() {
		atomic {
			state = STARTCOND;
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);//164; //TWINT | TWSTA | TWEN
		}//atomic
	}//TWI_Start

	uint8_t TWI_Status() {
		atomic {
			return TWSR & 0xF8; //only first 5 bits that we need
		}//atomic
	}//TWI_Status

  void TWI_Write(uint8_t data) {
		atomic {
			state = WRITEBYTE;
			TWDR = data;
			TWCR = (1 << TWINT) | (1 << TWEN);//132; //TWINT | TWEN
		}//atomic
	}//TWI_Write

	void TWI_Read(bool enableAck) {
		atomic {
			TWCR = (1 << TWINT) | (1 << TWEN);//132; //TWINT | TWEN
			if(enableAck) {
				TWCR |= (1 << TWEA);
			}//if
		}//atomic
	}//TWI_Read

	void TWI_Stop() {
		atomic {
			state = STOPCOND;
			TWCR = (1 << TWSTO) | (1 << TWEN);//148; //TWSTO | TWEN
		}//atomic
  }//TWI_Stop

//------------Init--------------
	command error_t Init.init() {
		TWI_Init();
		return SUCCESS;
	}

//-----------Splitcontrol-------
	command error_t SplitControl.start() {
		TWI_Start();
		return SUCCESS;
	}

	command error_t SplitControl.stop() {
		TWI_Stop();
		return SUCCESS;
	}

//----------I2CWrite------------
	command error_t I2CWrite.write(uint8_t data) {
		TWI_Write(data);
		return SUCCESS;
	}

//----------I2CRead-------------
	command error_t ReadAck.read() {
		TWI_Read(TRUE);
		return SUCCESS;
	}

	command error_t ReadNack.read() {
		TWI_Read(FALSE);
		return SUCCESS;
	}

//-----------ISR-----------------
	task void signalingTask() {
		switch(state) {
			case STARTCOND:
				atomic state = IDLE;
				signal SplitControl.startDone(TWI_Status());
				break;
			case STOPCOND:
				atomic state = IDLE;
				signal SplitControl.stopDone(TWI_Status());
				break;
			case WRITEBYTE:
				atomic state = IDLE;
				signal I2CWrite.writeDone(TWI_Status());
				break;
			case R_ACK:
				atomic state = IDLE;
				signal ReadAck.readDone(TWI_Status(), TWDR);
				break;
			case R_NACK:
				atomic state = IDLE;
				signal ReadNack.readDone(TWI_Status(), TWDR); 
				break;
		}//switch
	}

  AVR_ATOMIC_HANDLER(TWI_vect) {
 		post signalingTask();	 
  }//atomic_handler

	default event void SplitControl.startDone(error_t err) {}
	default event void SplitControl.stopDone(error_t err) {}
	default event void I2CWrite.writeDone(uint8_t stat) {}
	default event void ReadAck.readDone(uint8_t stat, uint8_t val) {}
	default event void ReadNack.readDone(uint8_t stat, uint8_t val) {}
}
