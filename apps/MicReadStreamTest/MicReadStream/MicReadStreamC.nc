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

#include "MicReadStream.h"

module MicReadStreamC{
	uses
	{
		interface ReadStream<uint16_t>;
		interface Timer<TMilli>;
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
	uint16_t MicRead[3][MIC_SAMPLES],sampleNum,bufferDoneNum,sendErrorNum,sendDoneErrorNum,busyTrueNum;
	uint32_t periodToSend;
	message_t dataMsg,readyMsg;
	bool busy;

	inline void dataSend(uint16_t*);
	inline void readySend(uint32_t);

	event void Boot.booted()
	{
		call SplitControl.start();
		call Leds.led0On();
		state=STOPPED;
		busy=FALSE;
	}

	event void SplitControl.startDone(error_t err)
	{
		if (err!=SUCCESS)
		{
			call SplitControl.start();
		}
	}

	event message_t* Receive.receive(message_t* message ,void* payload, uint8_t len)
	{
		uint8_t i;
				
		if (len == sizeof(ctrlmsg_t))
		{
			ctrlmsg_t *ctrl = (ctrlmsg_t*)payload;
			if (ctrl->instr == 's')
			{
				state = STARTED;
				sampleNum=0;
				bufferDoneNum=0;
				sendErrorNum=0;
				sendDoneErrorNum=0;
				busyTrueNum=0;
				for(i = 0; i < 3; ++i)
				{
					call ReadStream.postBuffer(MicRead[i], MIC_SAMPLES);
				}
				call ReadStream.read(ctrl->micPeriod);
				call Timer.startOneShot(TIMER_PERIOD);
			}
		}
		return message;
	}
	
	event void Timer.fired()
	{
		if(state == STOPPED)
		{
			readySend(periodToSend);
		}
		state=STOPPED;
		
	}

	event void ReadStream.bufferDone(error_t result, uint16_t* buf, uint16_t count)
	{
		if(result==SUCCESS)
		{
			bufferDoneNum++;
			if(busy)
			{
				busyTrueNum++;
			}	
			dataSend(buf);
			if(state == STARTED)
			{
				call ReadStream.postBuffer(buf, count);
			}
			
		}
	}
	
	event void ReadStream.readDone(error_t result, uint32_t usActualPeriod)
	{
		if(result==SUCCESS)
		{
			periodToSend = usActualPeriod;
			call Timer.startOneShot(64);
		}
	}
	
	event void DataSend.sendDone(message_t* bufPtr, error_t error)
	{
		if(error==SUCCESS)
		{
			busy=FALSE;
		}
		else
		{
			sendDoneErrorNum++;
			call Leds.led2Toggle();
		}
	}

	inline void dataSend(uint16_t *dataToSend)
	{
		uint8_t i;
		datamsg_t* packet;

		if(!busy)
		{
			busy=TRUE;
			packet=(datamsg_t*)(call DataSend.getPayload(&dataMsg, sizeof(datamsg_t)));
			for(i=0;i<MIC_SAMPLES;++i)
			{
				packet->micData[i]=dataToSend[i]>>2;
			}
			packet-> sampleNum = sampleNum++;
			if(call DataSend.send(AM_BROADCAST_ADDR, &dataMsg, sizeof(datamsg_t))!=SUCCESS)
			{
				sendErrorNum++;
				call Leds.led1Toggle();
			}
		}
	}

	inline void readySend(uint32_t usActualPeriod)
	{
		readymsg_t* packet = (readymsg_t*)(call DataSend.getPayload(&readyMsg, sizeof(readymsg_t)));
		packet->usActualPeriod=usActualPeriod;
		packet->sampleNum=sampleNum;
		packet->bufferDoneNum=bufferDoneNum;
		packet->sendErrorNum=sendErrorNum;
		packet->sendDoneErrorNum=sendDoneErrorNum;
		packet->busyTrueNum=busyTrueNum;
		call ReadySend.send(AM_BROADCAST_ADDR, &readyMsg, sizeof(readymsg_t));
	}

	event void ReadySend.sendDone(message_t* bufPtr, error_t error){}
	event void SplitControl.stopDone(error_t err){} 
}


