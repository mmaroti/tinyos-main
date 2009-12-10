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
#ifdef PLATFORM_TELOSB
#include <UserButton.h>	
#endif


module RSSINodeC
{
	uses
	{
		interface SplitControl;
		interface Boot;
		interface Leds;
		interface AMSend as intfSendUint8;
		interface AMSend as intfSendUint16;
		interface Receive;
		interface Timer<TMilli> as vrefTimer;
		interface Timer<TMilli> as ledTimer;
		interface Read<uint16_t> as Vref;

#ifdef PLATFORM_IRIS		
		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketLinkQuality;
#else
		interface CC2420Packet;
		interface AMSend as intfSendInt8;
#endif
#ifdef PLATFORM_TELOSB
	interface Notify<button_state_t>;
#endif
	}

}
implementation
{
	message_t msg;
	
	bool vrefRead;
	dataUint16_t vref;
	
	void calcUint16(uint16_t,dataUint16_t*, bool);
	void sendUint16(dataUint16_t*);
	void initUint16(dataUint16_t*);		
		
	void calcUint8(uint8_t, dataUint8_t*, bool);
	void sendUint8(dataUint8_t*);
	void initUint8(dataUint8_t*);
#ifndef PLATFORM_IRIS		
	void calcInt8(int8_t,dataInt8_t*,bool);
	void sendInt8(dataInt8_t*);
	void initInt8(dataInt8_t*);
#endif
	event void Boot.booted()
	{
		call SplitControl.start();
#ifdef PLATFORM_TELOSB
		call Notify.enable();
#endif
		initUint16(&vref);
	}
	
	event void SplitControl.startDone(error_t err)
	{
		if (err!=SUCCESS)
		{
			call SplitControl.start();
		}
	}
	
	event void SplitControl.stopDone(error_t err){}
	
	event message_t* Receive.receive(message_t* controlmsg ,void* payload, uint8_t len)
	{
#ifdef PLATFORM_IRIS		
		static dataUint8_t rssi;
#else
		static dataInt8_t rssi;
#endif		
		static dataUint8_t lqi;
		static bool initR=TRUE,initL=TRUE;
	
		if (len == sizeof(controlpacket_t))
		{
			controlpacket_t *ctrlpkt = (controlpacket_t*)payload;
			if (ctrlpkt->instr[0] == 's')
			{
				switch(ctrlpkt->instr[1])
				{
					case 'r':
#ifdef PLATFORM_IRIS
						if (initR)
						{
							initUint8(&rssi);
						}
						calcUint8(call PacketRSSI.get(controlmsg),&rssi,initR);
						rssi.dataType='r';
#else
						if (initR)
						{
							initInt8(&rssi);
						}	
						calcInt8(call CC2420Packet.getRssi(controlmsg),&rssi,initR);
						rssi.dataType='r';
#endif
						rssi.senderNodeID=ctrlpkt->nodeID;
						rssi.receiverNodeID=TOS_NODE_ID;
						initR=FALSE;
						break;
					case 'l':
						if (initL)
						{
							initUint8(&lqi);
						}	
#ifdef PLATFORM_IRIS		
						calcUint8(call PacketLinkQuality.get(controlmsg),&lqi,initL);
#else						
						calcUint8(call CC2420Packet.getLqi(controlmsg),&lqi,initL);
#endif						
						lqi.dataType='l';
						lqi.senderNodeID=ctrlpkt->nodeID;
						lqi.receiverNodeID=TOS_NODE_ID;
						initL=FALSE;
						break;
					case 'v':
						vrefRead = TRUE;
						call Vref.read();
						call vrefTimer.startOneShot(1024);
						vref.dataType='v';
						vref.senderNodeID=ctrlpkt->nodeID;
						vref.receiverNodeID=TOS_NODE_ID;
						break;
					case 'f':
						call ledTimer.startPeriodic(512);
						break;
				}
			}
			else if(ctrlpkt->instr[0] == 'g' && ctrlpkt->nodeID==TOS_NODE_ID)
			{
				switch(ctrlpkt->instr[1])
				{
					case 'r':
#ifdef PLATFORM_IRIS
						sendUint8(&rssi);
#else
						sendInt8(&rssi);
#endif
						initR=TRUE;
						break;
					case 'l':
						sendUint8(&lqi);
						initL=TRUE;
						break;
					case 'v':
						sendUint16(&vref);
						break;
				}
			}
		}
		return controlmsg;
	}
	
	event void vrefTimer.fired()
	{
		vrefRead=FALSE;
	}
	
	event void ledTimer.fired()
	{
		static uint8_t flash=0;
		
		if(flash==7)
		{
			call ledTimer.stop();
			flash=0;
		}
		else
		{
			flash++;
		}
		call Leds.set(flash);
	}
	
	event void Vref.readDone(error_t err, uint16_t data)
  	{
		if (err == SUCCESS)
	    {
     		calcUint16(data,&vref,!vrefRead);
     		if (vrefRead == TRUE)
	    	{
     			call Vref.read();
     		}
     	}
 	}
#ifndef PLATFORM_IRIS 	
 	event void intfSendInt8.sendDone(message_t* bufPtr, error_t error){}
#endif	
    event void intfSendUint8.sendDone(message_t* bufPtr, error_t error){}
  	
  	event void intfSendUint16.sendDone(message_t* bufPtr, error_t error){}
#ifdef PLATFORM_TELOSB
	event void Notify.notify( button_state_t state )
	{
		if ( state == BUTTON_PRESSED )
		{
			call Leds.led0On();
			call Leds.led1On();
			call Leds.led2On();
		}
		else if ( state == BUTTON_RELEASED )
		{
			call Leds.led0Off();
			call Leds.led1Off();
			call Leds.led2Off();
		}
	}
#endif	
#ifndef PLATFORM_IRIS 	
	void calcInt8(int8_t dataIn, dataInt8_t* structPtr, bool zeroLocals)
  	{
  		static int8_t firstVal=0,absDiff=0;
  		
  		if (zeroLocals==TRUE)
 		{
 			absDiff = 0;
 			firstVal= 0;
 		}
  		if (dataIn < structPtr->min)
 		{
 			structPtr->min = dataIn;
 		}
 		if (dataIn > structPtr->max)
 		{
 			structPtr->max = dataIn;
 		}
 		if (structPtr->sampleCnt >=1)
	  	{
	  		absDiff = abs(firstVal-dataIn);
	  	}
	  	firstVal = dataIn;
	  	structPtr->sum_e += absDiff;
	  	structPtr->sum_a += dataIn;
 		structPtr->sampleCnt++;
  	}
#endif  	
  	void calcUint16(uint16_t dataIn, dataUint16_t* structPtr,bool zeroLocals)
  	{
  		static uint16_t firstVal=0,absDiff=0;
  		
  		if (dataIn < structPtr->min)
 		{
 			structPtr->min = dataIn;
 		}
 		if (dataIn > structPtr->max)
 		{
 			structPtr->max = dataIn;
 		}
 		if (structPtr->sampleCnt >=1)
	  	{
	  		absDiff = abs(firstVal-dataIn);
	  	}
	  	firstVal = dataIn;
	  	structPtr->sum_e += absDiff;
	  	structPtr->sum_a += dataIn;
 		structPtr->sampleCnt++;
 		if (zeroLocals==TRUE)
 		{
 			absDiff = 0;
 			firstVal= 0;
   		}  		
  	}
  	
  	void calcUint8(uint8_t dataIn, dataUint8_t* structPtr, bool zeroLocals)
  	{
  		static uint8_t firstVal=0,absDiff=0;
  		
  		if (zeroLocals==TRUE)
 		{
 			absDiff = 0;
 			firstVal= 0;
   		}  		
  		if (dataIn < structPtr->min)
 		{
 			structPtr->min = dataIn;
 		}
 		if (dataIn > structPtr->max)
 		{
 			structPtr->max = dataIn;
 		}
 		if (structPtr->sampleCnt >=1)
	  	{
	  		absDiff = abs(firstVal-dataIn);
	  	}
	  	firstVal = dataIn;
	  	structPtr->sum_e += absDiff;
	  	structPtr->sum_a += dataIn;
 		structPtr->sampleCnt++;
  	}
#ifndef PLATFORM_IRIS  	
  	void sendInt8(dataInt8_t* structPtr)
  	{
  		memcpy(call intfSendInt8.getPayload(&msg, sizeof(dataInt8_t)),structPtr,sizeof(dataInt8_t));
  		call intfSendInt8.send(0, &msg, sizeof(dataInt8_t));
    }
#endif    
    void sendUint8(dataUint8_t* structPtr)
  	{
  		memcpy(call intfSendUint8.getPayload(&msg, sizeof(dataUint8_t)),structPtr,sizeof(dataUint8_t));
  		call intfSendUint8.send(0, &msg, sizeof(dataUint8_t));
    }
    
    void sendUint16(dataUint16_t *structPtr)
  	{

		memcpy(call intfSendUint16.getPayload(&msg, sizeof(dataUint16_t)),structPtr,sizeof(dataUint16_t));
  		call intfSendUint16.send(0, &msg, sizeof(dataUint16_t));
    }
#ifndef PLATFORM_IRIS  	    	
  	void initInt8(dataInt8_t* structPtr)
 	{
		structPtr->min=127;
		structPtr->max=-128;
		structPtr->sum_a=0;
		structPtr->sum_e=0;
		structPtr->sampleCnt=0;
	}
#endif
	void initUint8(dataUint8_t* structPtr)
	{

		structPtr->min=0xFF;
		structPtr->max=0;
		structPtr->sum_a=0;
		structPtr->sum_e=0;
		structPtr->sampleCnt=0;
	}

	void initUint16(dataUint16_t *structPtr)
	{

		structPtr->min=0xFFFF;
		structPtr->max=0;
		structPtr->sum_a=0;
		structPtr->sum_e=0;
		structPtr->sampleCnt=0;
	}	
}

