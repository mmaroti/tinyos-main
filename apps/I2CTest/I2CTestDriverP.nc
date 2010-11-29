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
	uses interface GeneralIO as I2CClk;
  uses interface GeneralIO as I2CData;

	uses interface DiagMsg;
	uses interface Led5 as Leds;
}
implementation {
	
	enum {
		IDLE = 0,
		STARTCOND,
		STOPCOND,
		WRITEBYTE,
		R_ACK,
		R_NACK,
		TWCR_def= (1 << TWEN) | (1 << TWINT) | (1 << TWIE) | (1 << TWEA),
	}	;
	
	uint8_t state = IDLE;

	void TWI_Init();
	void TWI_Start();
	uint8_t TWI_Status();
	void TWI_Read(bool);
	void TWI_Write(uint8_t);
	void TWI_Stop();

	inline void TWI_Init() {
		atomic {
			TWSR &= ~((uint8_t)3);
			//TWBR = (8000000 / 50000UL - 16) / 2;//48;
			TWBR = 72;
			TWCR = TWCR_def;//4; //TWEN
		}//atomic
	}//TWI_Init

	inline void TWI_Start() {
		atomic {
			state = STARTCOND;
			TWCR = TWCR_def | (1 << TWSTA);//164; //TWIE | TWSTA | TWEN
		}//atomic
	}//TWI_Start

	inline uint8_t TWI_Status() {
		atomic {
			return TWSR & 0xF8; //only first 5 bits that we need
		}//atomic
	}//TWI_Status

  inline void TWI_Write(uint8_t data) {
		atomic {
			state = WRITEBYTE;
			TWDR = data;
			TWCR = TWCR_def;//132; //TWIE | TWEN
		}//atomic
	}//TWI_Write

	inline void TWI_Read(bool enableAck) {
		//atomic {
			TWCR = TWCR_def;//132; //TWIE | TWEN
			if(enableAck) {
				TWCR |= (1 << TWEA);
			}//if
		//}//atomic
	}//TWI_Read

	inline void TWI_Stop() {
		atomic {
			state = STOPCOND;
			TWCR = TWCR_def | (1 << TWSTO);//148; //TWSTO | TWEN
		}//atomic
  }//TWI_Stop

//------------Init--------------
	command error_t Init.init() {
		DDRF |= _BV(PF2);
		PORTF |= _BV(PF2); //empowering i2c sensors for testing purposes
		call I2CClk.set();
		call I2CData.set();
		call I2CClk.makeInput();
		call I2CData.makeInput();
		TWI_Init();
		//call Leds.led0On();
		/*if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.init");
			call DiagMsg.uint8(state);
      call DiagMsg.send();
		}*/
		return SUCCESS;
	}

//-----------Splitcontrol-------
	command error_t SplitControl.start() {
		//TWI_Init();
		
		TWI_Start();
		if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.start");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		return SUCCESS;
	}

	command error_t SplitControl.stop() {
		TWI_Stop();
		if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.stop");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		return SUCCESS;
	}

//----------I2CWrite------------
	command error_t I2CWrite.write(uint8_t data) {
		TWI_Write(data);
		if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.write");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		return SUCCESS;
	}

//----------I2CRead-------------
	command error_t ReadAck.read() {
		TWI_Read(TRUE);
		if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.readAck");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		return SUCCESS;
	}

	command error_t ReadNack.read() {
		TWI_Read(FALSE);
		if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.raedNack");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		return SUCCESS;
	}

//-----------ISR-----------------
	task void signalingTask() {
	if(call DiagMsg.record()) {
			call DiagMsg.str("Dr.isr");
			call DiagMsg.uint8(state);
			call DiagMsg.send();
		}
		
		switch(state) {
			case STARTCOND:
				atomic state = IDLE;
				//call Leds.led0Toggle();
				signal SplitControl.startDone(TWI_Status());
				break;
			case STOPCOND:
				atomic state = IDLE;
				signal SplitControl.stopDone(TWI_Status());
				break;
			case WRITEBYTE:
				//call Leds.led1Toggle();
				atomic state = IDLE;
				signal I2CWrite.writeDone(TWI_Status());
				break;
			case R_ACK:
				//call Leds.led0Toggle();
				atomic state = IDLE;
				signal ReadAck.readDone(TWI_Status(), TWDR);
				break;
			case R_NACK:
				atomic state = IDLE;
				signal ReadNack.readDone(TWI_Status(), TWDR); 
				break;
		}//switch
		TWCR |= (1 << TWINT);
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
