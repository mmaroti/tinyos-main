/*
* Copyright (c) 2011, University of Szeged
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


#include "Bma180.h"
module ReaderC @safe()
{
  uses {
    interface Boot;
    interface Read<bma180_data_t>;
    interface ReadStream<bma180_data_t>;
    interface StdControl as BmaControl;
    interface Leds;
    interface AMSend;
    interface SplitControl as AMControl;
    interface DiagMsg;
    interface Init;
  }
}
implementation
{
  enum
	{
		BUFFER_COUNT = 3,
		BUFFER_SIZE = 10,
		SAMPLING = 56,
	};

  bma180_data_t buffers[BUFFER_COUNT][BUFFER_SIZE];
	message_t dataMsg;
 

  event void AMControl.startDone(error_t error) {
    uint8_t i;
    call BmaControl.start();
    call Init.init();
    call Read.read();
    

// 		for(i = 0; i < BUFFER_COUNT; ++i)
// 		{
// 			error = call ReadStream.postBuffer(buffers[i], BUFFER_SIZE);
// 		}
// 
// 		error = call ReadStream.read(SAMPLING);

  }

  event void Boot.booted() {
    //DDRF = _BV(PF0);
    //PORTF = _BV(PF0);
    
    call AMControl.start();
  }

  event void AMSend.sendDone(message_t* msg, error_t error) {}
  event void AMControl.stopDone(error_t error) {}
  event void Read.readDone(error_t result, bma180_data_t data) {
    if(call DiagMsg.record()) {
      call DiagMsg.str("TS: "); call DiagMsg.uint8(data.bma180_short_timestamp);
      call DiagMsg.str("X: ");
      call DiagMsg.int16( data.bma180_accel_x );
      call DiagMsg.str("Y: ");
      call DiagMsg.int16( data.bma180_accel_y );
      call DiagMsg.str("Z: ");
      call DiagMsg.int16( data.bma180_accel_z );
      call DiagMsg.str("Temp: ");
      call DiagMsg.uint8( (int8_t)data.bma180_temperature);
      call DiagMsg.send();
    } call Read.read();
  }

  event void ReadStream.bufferDone(error_t result, bma180_data_t* buf, uint16_t count) {
    uint8_t i;
		uint8_t *p;

// 		if( result == SUCCESS )
// 		{
// 			//sampleCount += BUFFER_SIZE;
// 
// 			p = call AMSend.getPayload(&dataMsg, BUFFER_SIZE);
// 			if( p != NULL )
// 			{
// 				for(i = 0; i < BUFFER_SIZE; ++i)
// 					p[i] = buf[i] >> 2;
// 
// 				result = call AMSend.send(AM_BROADCAST_ADDR, &dataMsg, BUFFER_SIZE);
// 				if( result != SUCCESS )
// 					call Leds.led0Toggle();
// 			}
// 
// 			call ReadStream.postBuffer(buf, count);
// 		}
  }


  event void ReadStream.readDone(error_t err, uint32_t usperiod) { int i;
    if(call DiagMsg.record()) {
      for(i=0; i<10; ++i)
      call DiagMsg.int16((buffers[2][i]).bma180_accel_z);
      call DiagMsg.send();
    }
  }
}
