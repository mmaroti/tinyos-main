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
#include "TimeSyncPoints.h"

module TimeSyncPointsP
{
	
	provides
	{
		interface TimeSyncPoints;
		interface SplitControl;
	}
	uses
	{	
		interface Receive as TimeSyncReceive;
		interface TimeSyncPacket<TMilli, uint32_t> as TimeSyncPacketMilli;
		interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
		interface TimeSyncAMSend<TMilli, uint32_t> as TimeSyncAMSendMilli;
		interface Timer<TMilli> as TimerMilli;
		interface SplitControl as AdjustRadio;		
	}
}
implementation
{
	enum
	{
		STOPPED=0,
		STARTING,
		STOPPING,
		STARTED,
	};
	uint8_t state = STOPPED;
	message_t syncMessage;
	command error_t SplitControl.start()
	{
		call AdjustRadio.start();
		state = STARTING;
		return SUCCESS;
	}
	
	command error_t SplitControl.stop()
	{
		call AdjustRadio.stop();
		state = STOPPING;
		return SUCCESS;
	}
	
	event void AdjustRadio.startDone(error_t err)
	{
		if (err==SUCCESS)
		{
			signal SplitControl.startDone(err);
			{
				if (state == STOPPING)
				{
					err=FAIL;
				}
				else if (state == STARTING)
				{
					err = SUCCESS;
					state = STARTED;
				}
				else if (state == STARTED)
				{
					err = EALREADY;
				}
			}
			if (state == STARTED)
			{
				call TimerMilli.startPeriodic(TIMESYNCPOINTS_PERIOD);
			}
		}
		else
		{
			call AdjustRadio.start();
		}
	}
		
	event void AdjustRadio.stopDone(error_t err)
	{
		if (err==SUCCESS)
		{
			signal SplitControl.stopDone(err);
			{
				if (state == STARTING)
				{
					err=FAIL;
				}
				else if (state == STOPPING)
				{
					err = SUCCESS;
					state = STOPPED;
				}
				else if (state == STOPPED)
				{
					err=EALREADY;
				}
			}
			if (state == STOPPED)
			{
				call TimerMilli.stop();
			}
		}
		else
		{
			call AdjustRadio.stop();
		}
	}
	
	event void TimerMilli.fired()
	{
		timeSyncPointsMsg_t* packet = (timeSyncPointsMsg_t*)(call TimeSyncAMSendMilli.getPayload(&syncMessage, sizeof(timeSyncPointsMsg_t)));
   		packet-> nodeID = TOS_NODE_ID;
   		call TimeSyncAMSendMilli.send(AM_BROADCAST_ADDR, &syncMessage, sizeof(timeSyncPointsMsg_t),0);
	}
	
	event void TimeSyncAMSendMilli.sendDone(message_t* Bufptr, error_t err)
	{
	}
	
	event message_t* TimeSyncReceive.receive(message_t* msg,void* payload, uint8_t len)
	{
		uint32_t localTime = 0;
		if (state == STARTED)
		{
			if ((call TimeSyncPacketMilli.isValid(msg)) && (call PacketTimeStampMilli.isValid(msg)))
			{
				timeSyncPointsMsg_t* syncMsg = (timeSyncPointsMsg_t*)payload;
				localTime=call PacketTimeStampMilli.timestamp(msg);
				signal TimeSyncPoints.syncPoint(localTime,syncMsg->nodeID,localTime-(call TimeSyncPacketMilli.eventTime(msg)));
			}
			return msg;
		}
		else
		{
			return msg;
		}
	}
}
 