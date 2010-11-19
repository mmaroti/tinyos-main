/*
 * Copyright (c) 2007, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Miklos Maroti
 */

#include <RadioAssert.h>

/*
 * Node 1 sends magic messages periodically to NodeID 2
 * Node 2 and 3 both respond with AutoAck to the same message
 * Node 2 and 3 uses frequence tuning triggered by the magic messages
 */

module TestP
{
	uses
	{
		interface SplitControl as SerialControl;
		interface SplitControl as RadioControl;
		interface Boot;

		interface DiagMsg;
		interface Timer<TMilli> as SendTimer;
		interface Leds;

		interface Receive;
		interface AMSend;
		interface PacketAcknowledgements;
		interface AMPacket;
		interface Packet;
	}
}

#ifndef SEND_PERIOD
#define SEND_PERIOD 20
#endif

implementation
{
	event void SerialControl.startDone(error_t error)
	{
		ASSERT( error == SUCCESS );
	}

	event void SerialControl.stopDone(error_t error)
	{
		ASSERT( error == SUCCESS );
	}

	event void RadioControl.startDone(error_t error)
	{
		ASSERT( error == SUCCESS );
	}

	event void RadioControl.stopDone(error_t error)
	{
		ASSERT( error == SUCCESS );
	}
	
	event void Boot.booted()
	{
		error_t error;

		error = call SerialControl.start();
		ASSERT( error == SUCCESS );

		error = call RadioControl.start();
		ASSERT( error == SUCCESS );

		call SendTimer.startPeriodic(SEND_PERIOD);
	}

	message_t txMsg;

	typedef struct ping_t
	{
		uint16_t seqNo;
		uint32_t magic;
	} ping_t;

	uint8_t tick = 0;

	event void SendTimer.fired()
	{
		ping_t* ping = (ping_t*)txMsg.data;
		error_t error;

		if( TOS_NODE_ID == 1 )
		{
			++tick;

			call Packet.clear(&txMsg);
			call PacketAcknowledgements.requestAck(&txMsg);

			ping->magic = 0xDEADBEAF;

			error = call AMSend.send(2, &txMsg, sizeof(ping_t));
			ASSERT( error == SUCCESS );

			call Leds.led0Toggle();
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		ping_t* ping = (ping_t*)txMsg.data;

		ASSERT( error == SUCCESS );

		++(ping->seqNo);
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if( TOS_NODE_ID == 2 || TOS_NODE_ID == 3 )
			call Leds.led1Toggle();

		return msg;
	}
}
