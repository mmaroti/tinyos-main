// $Id: DfrfBaseP.nc,v 1.3 2010-09-08 15:20:49 andrasbiro Exp $

/*									tab:4
 * "Copyright (c) 2000-2005 The Regents of the University  of California.  
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Copyright (c) 2002-2005 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */ 

/*
 * @author Phil Buonadonna
 * @author Gilman Tolle
 * @author David Gay
 * Revision:	$Id: DfrfBaseP.nc,v 1.3 2010-09-08 15:20:49 andrasbiro Exp $
 */ 

/* 
 * BaseStationP bridges packets between a serial channel and the radio.
 * Messages moving from serial to radio will be tagged with the group
 * ID compiled into the BaseStation, and messages moving from radio to
 * serial will be filtered by that same group id.
 */ 

#include "AM.h"
#include "Serial.h"
#include "StreamUploader.h"

module DfrfBaseP{
	uses {
		interface Boot;
		interface SplitControl as SerialControl;
		interface SplitControl as RadioControl;

		interface AMSend as CtrlSend;
		interface AMSend as DataSend;
        interface AMSend as TimeSend;
		interface Receive as GetReceive;
		interface Receive as CommandReceive;
		interface Packet as UartPacket;
		interface AMPacket as UartAMPacket;

		interface Packet as SerialPacket;
		interface AMPacket as SerialAMPacket;

		interface DfrfReceive as CtrlReceive;
		interface DfrfReceive as DataReceive;
		interface DfrfSend as GetSend;
		interface DfrfSend as CommandSend;

		interface Leds;
		
		interface LowPowerListening as LPL;
		interface SystemLowPowerListening as SysLPL;
		
		interface Pool<message_t>;
		interface Queue<message_t*>;
        
        interface TimeSyncPoints;
        interface LocalTime<TMilli>;
        interface StdControl as TimeSyncCtrl;
        interface Set<uint16_t> as SetInterval;
		
	}
}

implementation {
	message_t sendbuffer;
	
	inline void failBlink(){
		call Leds.led2Toggle();
	}
	
	inline void toSerialBlink(){
		call Leds.led1Toggle();
	}
	
	inline void toRadioBlink(){
		call Leds.led0Toggle();
	}

	event void Boot.booted() {
		call RadioControl.start();
	}

	event void RadioControl.startDone(error_t error) {
		if(error == SUCCESS) {
			call LPL.setLocalWakeupInterval(512);
			call SysLPL.setDefaultRemoteWakeupInterval(512);
			call SysLPL.setDelayAfterReceive(3072);
            call SetInterval.set(0);
            call TimeSyncCtrl.start();
			call SerialControl.start();
		} else 
			call RadioControl.start();
	}
	

	event void SerialControl.startDone(error_t error) {
		if(error != SUCCESS) 
			call SerialControl.start();
	}


	task void processQueue(){
		message_t *send = call Queue.dequeue();
		error_t error=FAIL;
		switch(call SerialAMPacket.type(send)){
			case AM_CTRL_MSG:{
				error=call CtrlSend.send(TOS_NODE_ID, send, sizeof(ctrl_msg));
			}break;
			case AM_DATA_MSG:{
				error=call DataSend.send(TOS_NODE_ID, send, sizeof(data_msg));
			}break;
            case AM_TIME_MSG:{
                time_msg *payload=(time_msg*)call TimeSend.getPayload(send,sizeof(time_msg));
                payload->sendTime=call LocalTime.get();
                error=call TimeSend.send(TOS_NODE_ID, send, sizeof(time_msg));
            }
		}
		if(error!=SUCCESS){
			call Queue.enqueue(send);
			post processQueue();
		}
	}
		
	inline void fillMessage(message_t *presendbuffer,void *payload, uint8_t len, am_id_t amtype, uint16_t source){
		memcpy(call SerialPacket.getPayload(presendbuffer, len),payload,len);	
		//call SerialAMPacket.setDestination(presendbuffer, TOS_NODE_ID);
		call SerialPacket.setPayloadLength(presendbuffer, len);
		call SerialAMPacket.setSource(presendbuffer, source);
		call SerialAMPacket.setType(presendbuffer, amtype);
		call Queue.enqueue(presendbuffer);
		if(call Queue.size()==1)
			post processQueue();
	}
	
	inline void sendDone(message_t *msg, error_t error){
		if(error==SUCCESS){
			call Pool.put(msg);
			toSerialBlink();
		} else
			call Queue.enqueue(msg);
		if(!call Queue.empty())
			post processQueue();
			
	}
	
	event void CtrlSend.sendDone(message_t *msg, error_t error){
		sendDone(msg,error);
	}
	
	event void DataSend.sendDone(message_t *msg, error_t error){
		sendDone(msg,error);
	}
	
	event void TimeSend.sendDone(message_t *msg, error_t error){
        sendDone(msg,error);
    }  
	
	event void TimeSyncPoints.syncPoint(uint32_t local, am_addr_t nodeId, uint32_t remote, uint16_t bootCount){
        message_t *presendbuffer=call Pool.get();
        if(presendbuffer!=NULL){
            time_msg data;//we can use local variable, becouse fillMessage is inline
            data.remoteTime=remote;
            data.localTime=local;
						data.bootCount=bootCount;
            fillMessage(presendbuffer, &data, sizeof(time_msg),AM_TIME_MSG,nodeId);
        } else
            failBlink();
    }
	
	event bool CtrlReceive.receive(void *data){
		message_t *presendbuffer=call Pool.get();
		if(presendbuffer!=NULL){
			ctrl_msg* mess=(ctrl_msg*)data;
			fillMessage(presendbuffer, data, sizeof(ctrl_msg),AM_CTRL_MSG,mess->source);
		} else
			failBlink();
		return TRUE;
	}

	event bool DataReceive.receive(void *data){
		message_t *presendbuffer=call Pool.get();
		if(presendbuffer!=NULL){
			data_msg* mess=(data_msg*)data;
			fillMessage(presendbuffer, data, sizeof(data_msg), AM_DATA_MSG, mess->source);
		} else
			failBlink();
		return TRUE;
	}

	event message_t * GetReceive.receive(message_t *msg, void *payload, uint8_t len){
		void *cached;
		cached=call GetSend.send(payload);
		return msg;
	}

	event message_t * CommandReceive.receive(message_t *msg, void *payload, uint8_t len){
		call CommandSend.send(payload);
		return msg;
	}
	
	event void SerialControl.stopDone(error_t error) {}
	event void RadioControl.stopDone(error_t error) {}
	
	inline void senddone(){
		toRadioBlink();
	}

	event void GetSend.sendDone(void *data){
		senddone();
	}

	event void CommandSend.sendDone(void *data){
		senddone();
	}
	


}