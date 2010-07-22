// $Id: SerialC.nc,v 1.1 2010-07-22 15:01:15 csepzol Exp $

/*
 * "Copyright (c) 2000-2006 The Regents of the University  of California.  
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
 */

/**
 * Implementation of the BlinkToRadio application.  A counter is
 * incremented and a radio message is sent whenever a timer fires.
 * Whenever a radio message is received, the three least significant
 * bits of the counter in the message payload are displayed on the
 * LEDs.  Program two motes with this application.  As long as they
 * are both within range of each other, the LEDs on both will keep
 * changing.  If the LEDs on one (or both) of the nodes stops changing
 * and hold steady, then that node is no longer receiving any messages
 * from the other node.
 *
 * @author Prabal Dutta
 * @date   Feb 1, 2006
 */
#include <Timer.h>
#include "BlinkToRadio.h"

module SerialC {
	uses interface Boot;
	uses interface Leds;
	uses interface SplitControl as AMControl;
	uses interface Receive;
	uses interface AMSend;
	uses interface Packet;
	uses interface AMSend as SerialAMSend;
	uses interface Receive as SerialReceive; 
	uses interface SplitControl as SerialControl;
}
implementation {

	bool busy = FALSE;
	bool locked = FALSE;
	uint16_t seged=0;
	message_t freeMsg;
	message_t *freeMsgPtr=&freeMsg;
	message_t pkt;
	 
	event void Boot.booted() {
		call SerialControl.start();
	}

	event void SerialControl.startDone(error_t err) {
		if (err == SUCCESS) {
			call AMControl.start();
		}
		else {
			call SerialControl.start();
		}
	}
	
	event void AMControl.startDone(error_t err) {
		if (err != SUCCESS) {
			call AMControl.start();	
		}
	}
	
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
		if (busy)
			return msg;
		if (call SerialAMSend.send(AM_BROADCAST_ADDR, msg, sizeof(BlinkToRadioMsg))!= SUCCESS)
			return msg;
			
		busy = TRUE;
		return freeMsgPtr;
	}
	
	event void SerialAMSend.sendDone(message_t* msg, error_t error) {
		freeMsgPtr=msg;
		busy = FALSE;
	}
	
	event message_t* SerialReceive.receive(message_t* msg, void* payload, uint8_t len) {
		if (len == sizeof(BlinkToRadioMsg)) {
			BlinkToRadioMsg* ptr = (BlinkToRadioMsg*)payload;
			call Leds.set(ptr->counter);
			seged=ptr->counter2;
			if (!locked){
				ControlMsg* btrpkt2 = (ControlMsg*)(call Packet.getPayload(&pkt, sizeof(ControlMsg)));
				btrpkt2->control=ptr->counter;
				if (call AMSend.send(seged, &pkt, sizeof(ControlMsg)) == SUCCESS) {
					locked = TRUE;
				}
			}
		}
		return msg;
	} 
	event void AMSend.sendDone(message_t* msg, error_t error) {
		if (error == SUCCESS) {
		locked=FALSE;
		//call Leds.led2Toggle();
		}
	}
	event void SerialControl.stopDone(error_t err) {}
	event void AMControl.stopDone(error_t err) {}
}





