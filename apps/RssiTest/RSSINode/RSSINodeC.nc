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
#include "Packet.h"

module RSSINodeC
{
	uses
	{
		interface SplitControl;
		interface Boot;
		interface Leds;
		interface Packet;
		interface AMSend;
		interface Receive;
		interface CC2420Packet;
	}

}
implementation
{
	message_t datamsg;
	
	event void Boot.booted()
	{
		call SplitControl.start();
	}
	
	event void SplitControl.startDone(error_t err)
	{
		if (err==SUCCESS)
		{
			//nothing to do
		}
		else
		{
			call SplitControl.start();
		}
	}
	
	event void SplitControl.stopDone(error_t err)
	{
		//no action
	}
	
	event message_t* Receive.receive(message_t* controlmsg ,void* payload, uint8_t len)
	{
		uint8_t lqi=0;
		int8_t rssi=0;
		static uint8_t lqiMin=0xFF,lqiMax=0,lqiAbsDiff=0,lqiFirstVal=0,nodeID=0;
		static int8_t rssiMin=0x7F,rssiMax=0,rssiAbsDiff=0,rssiFirstVal=0;
		static uint32_t lqiAver=0,lqiEnergy=0,sampleCnt=0;
		static int32_t rssiAver=0,rssiEnergy=0;

		if (len == sizeof(controlpacket_t))
		{
			controlpacket_t *ctrlpkt = (controlpacket_t*)payload;
			if (ctrlpkt->instr == 'r')
			{
				nodeID=ctrlpkt->nodeID;
				rssi = call CC2420Packet.getRssi(controlmsg);
				lqi = call CC2420Packet.getLqi(controlmsg);
				if (rssi < rssiMin)
 				{
 					rssiMin = rssi;
 				}
 				if (rssi > rssiMax)
 				{
 					rssiMax = rssi;
 				}
 				if (lqi < lqiMin)
 				{
 					lqiMin = lqi;
 				}
 				if (lqi > lqiMax)
 				{
 					lqiMax = lqi;
 				}
 				if (sampleCnt >=1)
	  			{
	  				rssiAbsDiff = abs(rssiFirstVal-rssi);
	  				lqiAbsDiff = abs(lqiFirstVal-lqi);
	  			}
	  			rssiFirstVal = rssi;
	  			lqiFirstVal = lqi; 
	  			rssiEnergy += rssiAbsDiff;
	  			lqiEnergy += lqiAbsDiff; 
 				rssiAver += rssi;
 				lqiAver +=lqi;
 				sampleCnt++;
				call Leds.led0Toggle();
			}
			else if (ctrlpkt->instr == 'g' && ctrlpkt->nodeID==TOS_NODE_ID)
			{
				datapacket_t* datapkt = (datapacket_t*)(call Packet.getPayload(&datamsg, (int)NULL));
    				datapkt-> senderNodeID = nodeID;
	    			datapkt-> receiverNodeID = TOS_NODE_ID;
    				datapkt-> sampleCnt = sampleCnt;
    				datapkt-> rssiMin = rssiMin;
	    			datapkt-> rssiAver = rssiAver;
    				datapkt-> rssiEnergy = rssiEnergy;
    				datapkt-> rssiMax = rssiMax;
    				datapkt-> lqiMin = lqiMin;
	    			datapkt-> lqiAver = lqiAver;
    				datapkt-> lqiEnergy = lqiEnergy;
    				datapkt-> lqiMax = lqiMax;
	    			if (call AMSend.send(0, &datamsg, sizeof(datapacket_t))==SUCCESS)
		    		{
    					rssiMin=0x7F;
    					rssiMax=0;
    					rssiAver=0;
	    				rssiEnergy=0;
    					rssiAbsDiff=0;
    					rssiFirstVal=0;
    					lqiMin=0xFF;
    					lqiMax=0;
	    				lqiAver=0;
    					lqiEnergy=0;
    					lqiAbsDiff=0;
    					lqiFirstVal=0;
    					sampleCnt=0;
	    			}
    				call Leds.led1Toggle();	
			}
			else
			{
				return controlmsg;
			}
			return controlmsg;
		}
		else
		{
			return controlmsg;
		}
	}

	event void AMSend.sendDone(message_t* bufPtr, error_t error)
	{
		if (&datamsg==bufPtr)
		{
	   		call Leds.led2Toggle();
		}
 	}
}