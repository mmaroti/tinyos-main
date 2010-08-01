// $Id: DfrfSTTestP.nc,v 1.3 2010-08-01 20:46:03 andrasbiro Exp $

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
 * Revision:	$Id: DfrfSTTestP.nc,v 1.3 2010-08-01 20:46:03 andrasbiro Exp $
 */ 

/* 
 * BaseStationP bridges packets between a serial channel and the radio.
 * Messages moving from serial to radio will be tagged with the group
 * ID compiled into the BaseStation, and messages moving from radio to
 * serial will be filtered by that same group id.
 */ 

#include "AM.h"
#include "Serial.h"
#include "CounterPacket.h"

module DfrfSTTestP @safe() {
	uses {
		interface Boot;
		interface SplitControl as SerialControl;
		interface SplitControl as RadioControl;

		interface AMSend as UartSend[am_id_t id];
		interface Packet as UartPacket;
		interface AMPacket as UartAMPacket;

		interface Packet as RadioPacket;
		interface AMPacket as RadioAMPacket;

		interface DfrfSend;
		interface DfrfReceive;
		
		interface DfrfSend as FieldSend;
		interface DfrfReceive as FieldReceive;
		interface Convergecast;

		interface Leds;
		interface Random;
		interface ParameterInit<uint16_t> as RandomSeedInit;
	}
}

implementation {
	enum {
		UART_QUEUE_LEN = 12,
		RADIO_QUEUE_LEN = 12,
	};

	message_t uartQueueBufs[UART_QUEUE_LEN];
	message_t * ONE_NOK uartQueue[UART_QUEUE_LEN];
	uint8_t uartIn, uartOut;
	bool uartBusy, uartFull;

	message_t radioQueueBufs[RADIO_QUEUE_LEN];
	message_t * ONE_NOK radioQueue[RADIO_QUEUE_LEN];
	message_t msg_temp;
	uint8_t radioIn, radioOut;
	bool radioBusy, radioFull;

	task void uartSendTask();

	void dropBlink() {
		call Leds.led2Toggle();
	}

	void failBlink() {
		call Leds.led2Toggle();
	}

	event void Boot.booted() {
		uint8_t i;

		for(i = 0; i < UART_QUEUE_LEN; i++) 
			uartQueue[i] = &uartQueueBufs[i];
		uartIn = uartOut = 0;
		uartBusy = FALSE;
		uartFull = TRUE;

		for(i = 0; i < RADIO_QUEUE_LEN; i++) 
			radioQueue[i] = &radioQueueBufs[i];
		radioIn = radioOut = 0;
		radioBusy = FALSE;
		radioFull = TRUE;

		call RadioControl.start();
		call SerialControl.start();
	}

	event void RadioControl.startDone(error_t error) {
		if(error == SUCCESS) {
			radioFull = FALSE;
			if(TOS_NODE_ID==0){
				counter_packet_t data;
				call RandomSeedInit.init(TOS_NODE_ID);
				data.src=call Random.rand16();
				data.data=0xbeaf;
				call FieldSend.send(&data);
			}
		}
	}

	event void SerialControl.startDone(error_t error) {
		if(error == SUCCESS) {
			uartFull = FALSE;
		}
	}

	event void SerialControl.stopDone(error_t error) {
	}
	
	event void RadioControl.stopDone(error_t error) {
	}
	
	message_t * receive(message_t * msg, void * payload, uint8_t len) {
		message_t * ret = msg;

		atomic {
			if( ! uartFull) {
				ret = uartQueue[uartIn];
				uartQueue[uartIn] = msg;

				uartIn = (uartIn + 1) % UART_QUEUE_LEN;

				if(uartIn == uartOut) 
					uartFull = TRUE;

				if( ! uartBusy) {
					post uartSendTask();
					uartBusy = TRUE;
				}
			}
			else 
				dropBlink();
		}

		return ret;
	}

	uint8_t tmpLen;

	task void uartSendTask() {
		uint8_t len;
		am_id_t id;
		am_addr_t addr, src;
		message_t * msg;
		am_group_t grp;
		atomic if(uartIn == uartOut && ! uartFull) {
			uartBusy = FALSE;
			return;
		}

		msg = uartQueue[uartOut];
		tmpLen = len = call RadioPacket.payloadLength(msg);
		id = call RadioAMPacket.type(msg);
		addr = call RadioAMPacket.destination(msg);
		src = call RadioAMPacket.source(msg);
		grp = call RadioAMPacket.group(msg);
		call UartPacket.clear(msg);
		call UartAMPacket.setSource(msg, src);
		call UartAMPacket.setGroup(msg, grp);
		tmpLen = len = call RadioPacket.payloadLength(msg);

		if(call UartSend.send[id](addr, uartQueue[uartOut], len) == SUCCESS) 
			call Leds.led1Toggle();
		else {
			failBlink();
			post uartSendTask();
		}
	}

	event void UartSend.sendDone[am_id_t id](message_t * msg, error_t error) {
		if(error != SUCCESS) 
			failBlink();
		else 
			atomic if(msg == uartQueue[uartOut]) {
			if(++uartOut >= UART_QUEUE_LEN) 
				uartOut = 0;
			if(uartFull) 
				uartFull = FALSE;
		}
		post uartSendTask();
	}
	
	void createMsg(message_t *msg, void *raw_data, uint8_t len, am_id_t type,am_group_t group, am_addr_t src, am_addr_t dest){
		memcpy(msg->data, raw_data,len);
		call RadioPacket.setPayloadLength(msg,len);
		call RadioAMPacket.setType(msg,type);
		call RadioAMPacket.setDestination(msg,dest);		
		call RadioAMPacket.setSource(msg,src);
		call RadioAMPacket.setGroup(msg,0);
	}

	event bool DfrfReceive.receive(void *raw_data){
		counter_packet_t *data=(counter_packet_t*)raw_data;
		createMsg(&msg_temp, raw_data, sizeof(counter_packet_t), APPID_COUNTER, 0, data->src, call RadioAMPacket.address());
		receive(&msg_temp, &msg_temp.data, sizeof(counter_packet_t));		
		return TRUE;
	}

	event bool FieldReceive.receive(void *raw_data){
		counter_packet_t *data=(counter_packet_t*)raw_data;
		counter_packet_t send;
		createMsg(&msg_temp, raw_data, sizeof(counter_packet_t), APPID_COUNTER, 0, data->src, call RadioAMPacket.address());
		receive(&msg_temp, &msg_temp.data, sizeof(counter_packet_t));		

		send.src=TOS_NODE_ID;
		send.data=call Convergecast.parent();
		call DfrfSend.send(&send);
		return TRUE;
	}
}