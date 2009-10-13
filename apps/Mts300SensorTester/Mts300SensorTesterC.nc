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

#include "Mts300SensorMsg.h"

module Mts300SensorTesterC{

	uses{
		interface Boot;
		interface Leds;
		interface Timer<TMilli> as TimerMilli;
		interface SplitControl as AMControl;
		interface AMSend as DataSend;
		interface Packet as DataPacket;
		interface Receive as ControlReceive;
		interface Mts300Sounder as Beeper; 
		interface Read<uint16_t> as PhotoRead;
		interface ReadStream<uint16_t> as MicRead;
		interface Read<uint16_t> as VrefRead;
		interface Read<uint16_t> as TempRead;
	}
}
implementation
{
  	uint32_t aver,energy;
  	uint16_t min,max,sampleCnt;
  	uint16_t micSamples[1000];
  	uint16_t micSampNum,micSampPer;
  	message_t msg;
  	bool readData;
  
	void send_data()
	{
		datamsg_t* packet = (datamsg_t*)(call DataPacket.getPayload(&msg, sizeof(datamsg_t)));
    	packet-> nodeID = TOS_NODE_ID;
	    packet-> min = min;
    	packet-> max = max;
	    packet-> average = aver;
		packet-> energy = energy;
		packet-> sampleCnt = sampleCnt;
		packet-> micSampPer = micSampPer;
		packet-> micSampNum = micSampNum;
    	call DataSend.send(0, &msg, sizeof(datamsg_t));
	}
	
	inline void Calculate_data(uint16_t data,bool zeroLocals)
 	{
 		static uint16_t abs_diff=0;
 		static uint16_t first_val=0;
 		
 		if (data < min)
 		{
 			min = data;
 		}
 		if (data > max)
 		{
 			max = data;
 		}
 		if (sampleCnt >=1)
	  	{
	  		abs_diff = abs(first_val-data);
	  	} 
	      	first_val = data;
	  	aver += data;
      		energy += abs_diff;
	  	sampleCnt++;
	  	if (zeroLocals==FALSE)
 		{
 			abs_diff = 0;
 			first_val= 0;
 		}
 	}

 	task void MicCountTask()
 	{
 		uint16_t abs_diff=0,first_val=0,data=0,count;
 		
 		for(count=0;count<micSampNum;count++)
 		{
 			data = micSamples[count];
 			if (data < min)
 			{
	 			min = data;
 			}
 			if (data > max)
 			{
	 			max = data;
 			}
	 		if (count >=1)
	  		{
	  			abs_diff = abs(first_val-data);
	  		} 
	      		first_val = data;
			aver += data;
      			energy += abs_diff;
	  	}
 	}
 
  	void Init_values()
 	{
  		aver = 0;
  		min = 0xFFFF;
  		max = 0;
  		energy = 0;
  		sampleCnt = 0;
  		micSampNum = 0;
  		micSampPer=0;
  		readData = TRUE;
  	}
 	
	event void Boot.booted()
	{
		Init_values();
		call AMControl.start();
		
	}

	event void AMControl.startDone(error_t err)
	{
		if(err == SUCCESS)
		{
			//nothing to do
		}
		else
		{
			call AMControl.start();
		}
	}

	event void AMControl.stopDone(error_t err)
	{
		//no action
	}
	
	event message_t* ControlReceive.receive(message_t* bufPtr,void* payload, uint8_t len)
	{
		if (len == sizeof(controlmsg_t))
		{
			controlmsg_t *ctrlmsg = (controlmsg_t*)payload;
			switch(ctrlmsg->instr)
			{
				case 'b':
					call Beeper.beep(300);
					break;
				case 'l':
					Init_values();
					call TimerMilli.startPeriodic(SAMP_TIME);
					call PhotoRead.read();
					break;	
				case 'm':
					Init_values();
					call MicRead.postBuffer(micSamples,1000);
					call MicRead.read(1);
					break;
				case 'v':
					Init_values();
					call TimerMilli.startPeriodic(SAMP_TIME);
					call VrefRead.read();
					break;
				case 't':
					Init_values();
					call TimerMilli.startPeriodic(SAMP_TIME);
					call TempRead.read();
					
					break;
				case 'g':
					send_data();
					break;
			}
		}
		return bufPtr;
	}	

	event void TimerMilli.fired()
	{
		call TimerMilli.stop();
		readData=FALSE;
	}
	
	event void PhotoRead.readDone(error_t err, uint16_t data)
 	{
		if (err == SUCCESS)
	    	{
     			Calculate_data(data,readData);
     			if (readData == TRUE)
	     		{
	     			call PhotoRead.read();
     			}
	      	}
 	}
 
 	event void MicRead.bufferDone(error_t result, uint16_t* bufPtr, uint16_t count)
	{
		if(result==SUCCESS)
		{
			micSampNum=count;
			post MicCountTask();
		}
	}

	event void MicRead.readDone(error_t result, uint32_t usActualPeriod)
	{
		if (result == SUCCESS)
		{
			micSampPer=usActualPeriod;
		}
	}

 	event void VrefRead.readDone(error_t err, uint16_t data)
  	{
		if (err == SUCCESS)
	    	{
     			Calculate_data(data,readData);
     			if (readData == TRUE)
	     		{
     				call VrefRead.read();
     			}
	      	}
 	}
 
 	event void TempRead.readDone(error_t err, uint16_t data)
  	{
		if (err == SUCCESS)
	    	{
     			Calculate_data(data,readData);
     			if (readData == TRUE)
	     		{
	     			call TempRead.read();
	     		}
	      	}
 	}

 	event void DataSend.sendDone(message_t* bufPtr, error_t error)
	{
	   // no action
  	}
}

