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
#include "TimeSyncUse.h"

module TimeSyncUseC
{
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli>as TimerMilli;
	uses interface SplitControl as TimeSyncControl;
	uses interface Random;
	uses interface Init;
	uses interface Packet; 
	uses interface AMSend;
	uses interface TimeSyncPoints;
}
implementation
{
	bool startSend;
	message_t timeMessage;
	uint8_t nodeId;
	uint32_t remoteTime,localTime;
	
	event void Boot.booted()
	{
		call TimeSyncControl.start();
		call Init.init();
		nodeId =0;
		remoteTime=0;
		localTime=0;
		startSend=FALSE;
	}
	
	event void TimeSyncControl.startDone(error_t err)
	{
		if (err == SUCCESS)
		{
			//nothing to do
		}
		else
		{
			call TimeSyncControl.start();
		}
	}
	
	event void TimeSyncControl.stopDone(error_t err)
	{
		//nothing to do
	}
	
	event void TimeSyncPoints.syncPoint(uint32_t locTime,uint8_t ID, uint32_t remTime)
	{
		uint8_t waitTime=0;
		if (!startSend)
		{
			startSend = TRUE;
			nodeId=ID;
			remoteTime=remTime;
			localTime=locTime;
			waitTime=call Random.rand16();
			call TimerMilli.startOneShot(waitTime);
			call Leds.led0Toggle();
		}
	}
	
	event void TimerMilli.fired()
	{
		timemsg_t *pkt = (timemsg_t*)(call Packet.getPayload(&timeMessage,sizeof(timemsg_t)));
		pkt->remNodeID=nodeId;
		pkt->remTime=remoteTime;
		pkt->locNodeID=TOS_NODE_ID;
		pkt->locTime=localTime;
		if (call AMSend.send(0,&timeMessage, sizeof(timemsg_t))==SUCCESS)
		{
			startSend = FALSE;
		}
	}
	
	event void AMSend.sendDone(message_t* bufPtr,error_t err)
	{
	}
}
 