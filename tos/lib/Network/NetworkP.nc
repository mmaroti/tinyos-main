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
* Author:Andras Biro
*/
#include "Network.h"
module NetworkP{
	provides interface StdControl;
	provides interface TimeSyncPoints; 
	provides interface AMSend[am_id_t am_id];
	provides interface Receive[am_id_t am_id];
	provides interface Receive as Snoop[am_id_t am_id];
	uses {
		interface TimeSyncAMSend<TMilli, uint32_t>;
		interface Receive as TimeSyncReceive;
		interface TimeSyncPacket<TMilli, uint32_t>;
		interface AMPacket as TimeSyncAMPacket;
		interface Packet;
		interface AMPacket;
		interface LocalTime<TMilli>;
		interface Timer<TMilli> as ListenTimer;
		interface Timer<TMilli> as FirstSendTimer;
		interface RadioScheduler as DiscoveryScheduler;
		interface RadioScheduler as BeaconScheduler;
		interface RadioScheduler as NetworkScheduler[uint8_t user_id];
		interface Receive as SubReceive;
		interface AMSend as SubSend;
		interface Receive as SubSnoop;
		interface Random;
		interface Init as RandomInit;
		interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
		interface Leds;
	}
}
implementation{

	typedef nx_struct{
		nx_uint32_t localTime;
		nx_uint8_t flags;
	} beacon_t;
	
	typedef struct{
		uint16_t nodeid;
		uint32_t nextwake;
		uint32_t nextsleep;
		uint8_t lost;//lost=0: lost, lost=0xfe..x: didn't hear from the mote for 'lost' cycles, lost=0xff: present 
	} negihbor;
	
	uint32_t nextBeacon;
	bool init=TRUE;
	message_t beacon, *data;
	
	am_addr_t wakenBy=TOS_BCAST_ADDR;
	negihbor nt[MAX_NEIGHBOR];
	
	command error_t StdControl.stop(){
		return SUCCESS;
		// TODO Auto-generated method stub
	}

	command error_t StdControl.start(){
		call RandomInit.init();
		call BeaconScheduler.RadioStart(call LocalTime.get());//send the first message ASAP
		call DiscoveryScheduler.RadioStart(call LocalTime.get());
		return SUCCESS;
	}

	command uint8_t AMSend.maxPayloadLength[am_id_t am_id](){
		return call SubSend.maxPayloadLength()-1;
	}

	command void * AMSend.getPayload[am_id_t am_id](message_t *msg, uint8_t len){
		return call SubSend.getPayload(msg, len);
	}

	command error_t AMSend.send[am_id_t am_id](am_addr_t addr, message_t *msg, uint8_t len){
		msg->data[len]=am_id;
		len++;
		if(call ListenTimer.isRunning()){
			return call SubSend.send(addr, msg, len);
		}else{
			beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
			mess->flags|=FLAG_DATA;
			data=msg;
			call Packet.setPayloadLength(data, len);
			call AMPacket.setDestination(msg, addr);
			//call AMPacket.setType(msg, am_id);
			return SUCCESS;	
		}
	}

	command error_t AMSend.cancel[am_id_t am_id](message_t *msg){
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		if((mess->flags&FLAG_DATA)!=0){
			mess->flags&=~FLAG_DATA;
			return SUCCESS;
		} else {
			return call SubSend.cancel(msg);
		}
	}
	
	event void SubSend.sendDone(message_t *msg, error_t error){
		if(error==SUCCESS){
			call ListenTimer.startOneShot(BEACON_INTERVAL);
			//printf("send done, sleep at %ld (%ld)\n",call ListenTimer.gett0()+call ListenTimer.getdt(), call ListenTimer.getNow());
		}
		signal AMSend.sendDone[msg->data[call Packet.payloadLength(msg)-1]](msg, error);
	}

	event message_t * SubReceive.receive(message_t *msg, void *payload, uint8_t len){
//		printf("rec from %u, waken by: %u\n",call AMPacket.source(msg),wakenBy);
//		printfflush();
		if(call AMPacket.source(msg)==wakenBy){
			uint8_t i;
			call Leds.led2Toggle();
			for(i=0;i<MAX_NEIGHBOR;i++){
				if(nt[i].nodeid==call TimeSyncAMPacket.source(msg))
					break;
			}
			if(i!=MAX_NEIGHBOR){
				nt[i].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
				if(!call NetworkScheduler.RadioStopCancel[i]()==SUCCESS)
					call NetworkScheduler.RadioStart[i](call LocalTime.get());//TODO: move this to a task: if the radio is currently switching of, we can't turn it on 
	//			printf("next sleep: %ld (%ld)\n",nt[i].nextsleep,call LocalTime.get());
	//			printfflush();
				
			}
		}
		signal Receive.receive[msg->data[call Packet.payloadLength(msg)-1]](msg, payload, len-1);
		return msg;
	}
	
	event message_t * SubSnoop.receive(message_t *msg, void *payload, uint8_t len){
		if(call AMPacket.source(msg)==wakenBy){
			uint8_t i;
			for(i=0;i<MAX_NEIGHBOR;i++){
				if(nt[i].nodeid==call TimeSyncAMPacket.source(msg))
					break;
			}
			if(i!=MAX_NEIGHBOR){
				nt[i].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
				if(!call NetworkScheduler.RadioStopCancel[i]()==SUCCESS)
					call NetworkScheduler.RadioStart[i](call LocalTime.get());//TODO: move this to a task: if the radio is currently switching of, we can't turn it on 
	//			printf("next sleep: %ld (%ld)\n",nt[i].nextsleep,call LocalTime.get());
	//			printfflush();
				
			}
		}
		signal Snoop.receive[msg->data[call Packet.payloadLength(msg)-1]](msg, payload, len-1);
		return msg;
	}

	event void DiscoveryScheduler.RadioStartDone(error_t error){
		if(error!=SUCCESS){
			call DiscoveryScheduler.RadioStart(call LocalTime.get());
		} else {
			if(init){
				init=FALSE;
				call DiscoveryScheduler.RadioStop(call LocalTime.get()+INIT_DISCOVERY);
			} else {
				call DiscoveryScheduler.RadioStop(call LocalTime.get()+BEACON_INTERVAL);
			}
		}
	}

	event void DiscoveryScheduler.RadioStopDone(error_t error){
		if(error!=SUCCESS){
			call DiscoveryScheduler.RadioStop(call LocalTime.get());
		} else {
			//TODO: randomize the waiting time
			call DiscoveryScheduler.RadioStart(call LocalTime.get()+INIT_DISCOVERY);
		}
	}

	event void BeaconScheduler.RadioStartDone(error_t error){
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		mess->localTime=call LocalTime.get();
		//call BeaconScheduler.RadioStop(mess->localTime+10);
		if(nextBeacon==0)
			nextBeacon=mess->localTime+BEACON_INTERVAL;
		else
			nextBeacon+=BEACON_INTERVAL;
		if(call TimeSyncAMSend.send(TOS_BCAST_ADDR, &beacon, sizeof(beacon_t), mess->localTime)!=SUCCESS)
			call BeaconScheduler.RadioStop(mess->localTime);
	}
	
	event void TimeSyncAMSend.sendDone(message_t *msg, error_t error){
		uint8_t i=0;
		uint32_t localtime=call LocalTime.get();
		beacon_t* mess=call Packet.getPayload(&beacon, sizeof(beacon_t));
		if((mess->flags&FLAG_DATA)==0)
			call BeaconScheduler.RadioStop(localtime);
		else {
			mess->flags&=~FLAG_DATA;
			call ListenTimer.startOneShot(BEACON_INTERVAL);
			printf("start send, sleep at %ld (%ld)\n",call ListenTimer.gett0()+call ListenTimer.getdt(), call ListenTimer.getNow());
			//call FirstSendTimer.startOneShot(100);
			call SubSend.send(call AMPacket.destination(data), data, call Packet.payloadLength(data));
		}
		for(i=0;i<MAX_NEIGHBOR;i++){
			if(nt[i].lost>0){
				nt[i].lost--;
				if(nt[i].lost<LOST_LIMIT){
					nt[i].lost=0;
				}else{
					while(nt[i].nextwake<localtime){
						nt[i].nextwake+=BEACON_INTERVAL;
					}
					call NetworkScheduler.RadioStart[i](nt[i].nextwake);
				}
			}
		}
		
	}
	

	event void BeaconScheduler.RadioStopDone(error_t error){
		if(error!=SUCCESS){
			call BeaconScheduler.RadioStop(call LocalTime.get());
		} else {
			call BeaconScheduler.RadioStart(nextBeacon);
		}	
	}
	
	event void NetworkScheduler.RadioStartDone[uint8_t user_id](error_t error){
		if(wakenBy==TOS_BCAST_ADDR){
			nt[user_id].nextsleep=call LocalTime.get()+WAKE_TIME;
			call NetworkScheduler.RadioStop[user_id](nt[user_id].nextsleep);//this should be enough to receive a message
		} else {
			printf("waken by (with restart) %d (%d))\n",wakenBy, user_id);
			printfflush();
			nt[user_id].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
			call NetworkScheduler.RadioStop[user_id](nt[user_id].nextsleep);
		}
	}

	event void NetworkScheduler.RadioStopDone[uint8_t user_id](error_t error){
		if(error==ECANCEL){
			printf("waken by (with cancel) %d (%d)\n",wakenBy, user_id);
			printfflush();
			nt[user_id].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
			call NetworkScheduler.RadioStop[user_id](nt[user_id].nextsleep);
		} else
			wakenBy=TOS_BCAST_ADDR;
	}

	event message_t * TimeSyncReceive.receive(message_t *msg, void *payload, uint8_t len){
		uint8_t i=0;
		beacon_t* mess=call Packet.getPayload(msg, sizeof(beacon_t));
//		printf("%u\n", mess->flags);
//		printfflush();
		while(i<MAX_NEIGHBOR){
			if(nt[i].nodeid==call TimeSyncAMPacket.source(msg))
				break;
			i++;
		}
		if(i!=MAX_NEIGHBOR){//refresh the data of a neighbor
			if(call TimeSyncPacket.isValid(msg)){
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} else
				nt[i].nextwake+=BEACON_INTERVAL;
			if((mess->flags&FLAG_DATA)!=0){
//				printf("sm: %u (%u)\n",call NetworkScheduler.RadioStopModify[i](nt[i].nextsleep), mess->flags);
				wakenBy=call AMPacket.source(msg);
				if(!call NetworkScheduler.RadioStopCancel[i]()==SUCCESS)
					call NetworkScheduler.RadioStart[i](call LocalTime.get());//TODO: move this to a task: if the radio is currently switching of, we can't turn it on 
				//nt[i].nextsleep=call LocalTime.get()+BEACON_INTERVAL;
				//call NetworkScheduler.RadioStopModify[i](nt[i].nextsleep);
			}
			nt[i].lost=0xff;
		} else if(call TimeSyncPacket.isValid(msg)){//if the timestamp is not valid, we can't insert the source as a new neighbor
			uint32_t farestwaketime=0;
			uint8_t farestwaketimeuser=MAX_NEIGHBOR;
			i=0;
			while(i<MAX_NEIGHBOR){
				if(nt[i].lost==0)
					break;
				if(nextBeacon-nt[i].nextwake>farestwaketime){
					farestwaketime=nextBeacon-nt[i].nextwake;
					farestwaketimeuser=i;
				}
				i++;
			}
			if(i!=MAX_NEIGHBOR){//insert a new neighbor into an empty place
				nt[i].lost=0xff;
				nt[i].nodeid=call TimeSyncAMPacket.source(msg);
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} else if(farestwaketimeuser!=MAX_NEIGHBOR||call TimeSyncAMPacket.source(msg)<=MAX_BS_NODEID){ //insert a new neighbor instead of a worse one, or insert a basestation instead of the worst one
				nt[i].lost=0xff;
				nt[i].nodeid=call TimeSyncAMPacket.source(msg);
				nt[i].nextwake=call TimeSyncPacket.eventTime(msg)+BEACON_INTERVAL;
			} 
		}
		if(call TimeSyncPacket.isValid(msg)&&call PacketTimeStampMilli.isValid(msg)){
			signal TimeSyncPoints.syncPoint(call PacketTimeStampMilli.timestamp(msg), call TimeSyncAMPacket.source(msg), call TimeSyncPacket.eventTime(msg)+mess->localTime);
		}
		return msg;
	}
	
	event void ListenTimer.fired(){
		uint32_t localtime=call LocalTime.get();
		printf("LT fired at %ld\n",localtime);
		printfflush();
		call Leds.led1Toggle();
		while((int32_t)(localtime-nextBeacon)>0){
			nextBeacon+=BEACON_INTERVAL;
		}
		call BeaconScheduler.RadioStop(localtime);
	}
	
	default event message_t * Receive.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		return msg;
	};
	
	default event message_t * Snoop.receive[am_id_t am_id](message_t *msg, void *payload, uint8_t len){
		return msg;
	};
	
	default event void AMSend.sendDone[am_id_t am_id](message_t *msg, uint8_t len){};
	

	event void FirstSendTimer.fired(){
		printf("FS fired at %ld\n",call LocalTime.get());
		printfflush();
		call Leds.led2Toggle();
		call SubSend.send(call AMPacket.destination(data), data, call Packet.payloadLength(data));
	}
}