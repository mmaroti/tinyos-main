/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
* Author: Denes Paczolay
* Author: Gabor Gosztolya
*/

#include "MicReadStream.h"

module MicReadStreamC{
	uses
	{
		interface ReadStream<uint16_t>;
		interface MicSetting;
		interface Leds;
		interface AMSend;
		interface Boot;
		interface SplitControl;
	}
}
implementation
{
#ifdef EIGHT_BIT_DATA
#define BUFFER_SIZE 110
#else
#define BUFFER_SIZE 88
#endif

	uint16_t MicRead[3][BUFFER_SIZE],bufferNum,gainVal;	
	message_t dataMsg;

	event void Boot.booted()
	{
		call SplitControl.start();
	}

	event void SplitControl.startDone(error_t err)
	{
		uint8_t i;
		
		if (err==SUCCESS)
		{
			for(i = 0; i < 3; ++i)
			{
				call ReadStream.postBuffer(MicRead[i], BUFFER_SIZE);
			}
//			call ReadStream.read(56);
			call ReadStream.read(128);
		}
		else
		{
			call SplitControl.start();
		}
	}

	event void ReadStream.bufferDone(error_t result, uint16_t* buf, uint16_t count)
	{

#ifndef EIGHT_BIT_DATA
		uint8_t j;
#endif
		uint8_t i;
		datamsg_t* packet;
		
		if(result==SUCCESS)
		{
			bufferNum++;
			packet=(datamsg_t*)(call AMSend.getPayload(&dataMsg, sizeof(datamsg_t)));
#ifdef EIGHT_BIT_DATA
			for(i=0;i<BUFFER_SIZE;++i)
			{
				////////////////////////////////////////////////////////////////////
				//Encode the microphone data to remove the 0x7e, 0x7d, characters //
				//These characters are used as a flag in the serial transmission  //
				////////////////////////////////////////////////////////////////////
				packet->data[i]=(buf[i]>>2)^0x80;
			}
#else
			i = j = 0;
			for(i = 0; i < BUFFER_SIZE; i += 4)
			{
				packet->data[j    ] = (uint8_t)buf[i    ]; 
				packet->data[j + 1] = (uint8_t)buf[i + 1]; 
				packet->data[j + 2] = (uint8_t)buf[i + 2]; 
				packet->data[j + 3] = (uint8_t)buf[i + 3];
				packet->data[j + 4] = (uint8_t)(
					((buf[i    ] >> 8) &   3) |  
					((buf[i + 1] >> 6) &  12) |  
					((buf[i + 2] >> 4) &  48) |  
					((buf[i + 3] >> 2) & 192) );
				j += 5;
			}
#endif
			if(bufferNum%100==0)
			{
				gainVal=255-gainVal;
//				call MicSetting.gainAdjust(gainVal);
			}
			packet->bufferNum=bufferNum;
			packet->gainVal=gainVal;	
			if(call AMSend.send(AM_BROADCAST_ADDR, &dataMsg, sizeof(datamsg_t))!=SUCCESS)
			{
				call Leds.led0Toggle();
			}	
			call ReadStream.postBuffer(buf, count);
		}
	}

	event void AMSend.sendDone(message_t* bufPtr, error_t error)
	{
		if(error!=SUCCESS)
		{
			call Leds.led1Toggle();
		}
	}
	event void ReadStream.readDone(error_t result, uint32_t usActualPeriod){
		if (result == SUCCESS)
		{
			call Leds.led2On();
		}
	}

	async event error_t MicSetting.toneDetected()
	{
		return FALSE;
	}
	
	event void SplitControl.stopDone(error_t err){}
}


