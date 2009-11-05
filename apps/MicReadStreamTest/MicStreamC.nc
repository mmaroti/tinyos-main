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
*/

#include "MicStream.h"

module MicStreamC @ safe(){
	
	uses
	{
		interface ReadStream<uint16_t> as MicRead;
		interface Timer<TMilli> as Timer;
		interface Leds;
		interface AMSend as DataSend;
		interface AMSend as ReadySend;
		interface Receive;
		interface Boot;
		interface SplitControl;
	}
}
implementation
{
	enum
	{
		STOPPED=0,
		STARTED,
	};
	
	uint8_t state;
	uint16_t micData[2][MIC_SAMPLES];
	message_t dataMsg,readyMsg;
	bool busy;

	inline void dataSend(uint16_t *dataToSend)
	{
		if(!busy)
		{
			datamsg_t* packet = (datamsg_t*)(call DataSend.getPayload(&dataMsg, sizeof(datamsg_t)));
    		packet-> micData[0] = dataToSend[0];
    		packet-> micData[1] = dataToSend[1];
    		packet-> micData[2] = dataToSend[2];
    		packet-> micData[3] = dataToSend[3];
    		packet-> micData[4] = dataToSend[4];
    		packet-> micData[5] = dataToSend[5];
    		packet-> micData[6] = dataToSend[6];
    		packet-> micData[7] = dataToSend[7];
    		packet-> micData[8] = dataToSend[8];
    		packet-> micData[9] = dataToSend[9];
	    	if(call DataSend.send(AM_BROADCAST_ADDR, &dataMsg, sizeof(datamsg_t))==SUCCESS)
        	{
        		busy=TRUE;
        	}
        }
    }
    inline void readySend(uint32_t usActualPeriod)
    {
		readymsg_t* packet = (readymsg_t*)(call DataSend.getPayload(&readyMsg, sizeof(readymsg_t)));
		packet->usActualPeriod=usActualPeriod;
		call ReadySend.send(AM_BROADCAST_ADDR, &readyMsg, sizeof(readymsg_t));
	}

//	void sendMessage(uint32_t usActualPeriod,uint32_t sampleNum)
//	{
//		micMsg.sampleNum=sampleNum;
//		micMsg.usActualPeriod=usActualPeriod;
		
//		memcpy(call AMSend.getPayload(&msg, sizeof(micmsg_t)),&micMsg,sizeof(micmsg_t));
//    	call AMSend.send(AM_BROADCAST_ADDR, &msg, sizeof(micmsg_t));
//   }

	event void Boot.booted()
	{
		call SplitControl.start();
		busy=FALSE;
		state=STOPPED;
	}
	
	event void SplitControl.startDone(error_t err)
	{
		if (err!=SUCCESS)
		{
			call SplitControl.start();
		}
	}
	
	event void SplitControl.stopDone(error_t err)
	{
	} 
	
	event message_t* Receive.receive(message_t* message ,void* payload, uint8_t len)
	{
		if (len == sizeof(ctrlmsg_t))
		{
			ctrlmsg_t *ctrl = (ctrlmsg_t*)payload;
			if (ctrl->instr == 's')
			{
				state = STARTED;
				call MicRead.postBuffer(&micData[0][0],MIC_SAMPLES);
				call MicRead.postBuffer(&micData[1][0],MIC_SAMPLES);
				if(call MicRead.read(ctrl->micPeriod)!=SUCCESS)
				{
					call Leds.led0On();
				}
				call Timer.startOneShot(TIMER_PERIOD);
			}
		}
	    return message;
     }
	
	event void Timer.fired()
	{
		state=STOPPED;
	}

	event void MicRead.bufferDone(error_t result, uint16_t* buf, uint16_t count)
	{
		static bool firstBuffEn=TRUE;
		
		if(result==SUCCESS)
		{
			if (state==STARTED)
			{
				dataSend(buf);
				if(firstBuffEn)
				{
					firstBuffEn=FALSE;
					call MicRead.postBuffer(&micData[0][0],MIC_SAMPLES);
				}
				else
				{
					firstBuffEn=TRUE;
					call MicRead.postBuffer(&micData[1][0],MIC_SAMPLES);
				}
			}
			else
			{
				firstBuffEn = TRUE;
			}
		}
		else
		{
			call Leds.led1On();
		}			
	}
	
	event void MicRead.readDone(error_t result, uint32_t usActualPeriod)
	{
		if(result==SUCCESS)
		{
			readySend(usActualPeriod);
			call Leds.led2Toggle();
		}
	}

	event void DataSend.sendDone(message_t* bufPtr, error_t error)
	{
		if(error==SUCCESS)
		{
			busy=FALSE;
		}
  	}
  	
  	event void ReadySend.sendDone(message_t* bufPtr, error_t error)
	{
	}
}

