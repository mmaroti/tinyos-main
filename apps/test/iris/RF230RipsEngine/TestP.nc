/*
* Copyright (c) 2009, University of Szeged
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
* Author: Miklos Maroti
*/

#include <RadioAssert.h>
#include "RF230RipsEngine.h"

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
		interface Packet;
	}
}

#ifndef SEND_PERIOD
#define SEND_PERIOD 1000
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
/*
	uint8_t config[][5] = { 
			{ 1, 2, 3, 0x00, 0x00 },
			{ 2, 3, 4, 0x00, 0x00 },
			{ 3, 4, 1, 0x0C, 0x00 },
			{ 4, 1, 2, 0x00, 0x0C },
			{ 1, 2, 3, 0x10, 0x00 },
			{ 2, 3, 4, 0x10, 0x00 },
			{ 3, 4, 1, 0x1C, 0x00 },
			{ 4, 1, 2, 0x10, 0x0C },
			{ 1, 2, 3, 0x20, 0x00 },
			{ 2, 3, 4, 0x20, 0x00 },
			{ 3, 4, 1, 0x2C, 0x00 },
			{ 4, 1, 2, 0x20, 0x0C },
			{ 1, 2, 3, 0x00, 0x10 },
			{ 2, 3, 4, 0x00, 0x10 },
			{ 3, 4, 1, 0x0C, 0x10 },
			{ 4, 1, 2, 0x00, 0x1C },
			{ 1, 2, 3, 0x10, 0x10 },
			{ 2, 3, 4, 0x10, 0x10 },
			{ 3, 4, 1, 0x1C, 0x10 },
			{ 4, 1, 2, 0x10, 0x1C },
			{ 1, 2, 3, 0x20, 0x10 },
			{ 2, 3, 4, 0x20, 0x10 },
			{ 3, 4, 1, 0x2C, 0x10 },
			{ 4, 1, 2, 0x20, 0x1C },
			{ 1, 2, 3, 0x00, 0x20 },
			{ 2, 3, 4, 0x00, 0x20 },
			{ 3, 4, 1, 0x0C, 0x20 },
			{ 4, 1, 2, 0x00, 0x2C },
			{ 1, 2, 3, 0x10, 0x20 },
			{ 2, 3, 4, 0x10, 0x20 },
			{ 3, 4, 1, 0x1C, 0x20 },
			{ 4, 1, 2, 0x10, 0x2C },
			{ 1, 2, 3, 0x20, 0x20 },
			{ 2, 3, 4, 0x20, 0x20 },
			{ 3, 4, 1, 0x2C, 0x20 },
			{ 4, 1, 2, 0x20, 0x2C },
		};
*/

	uint8_t config[][5] = { 
			{ 4, 2, 3, 0xe0, 0xe0 },
		};


	enum { CONFIGURATIONS = sizeof(config)/5 };

	uint8_t configIndex;

	event void SendTimer.fired()
	{
		if( TOS_NODE_ID == 1 )
		{
			error_t error;
			rips_start_message_t* ripsMsg;

			call Packet.clear(&txMsg);
			ripsMsg = call Packet.getPayload(&txMsg, sizeof(rips_start_message_t));
			ASSERT( ripsMsg != 0 );

			++(ripsMsg->measurement_id);

			ripsMsg->beacon_nodeid = config[configIndex][0];
			ripsMsg->assist1_nodeid = config[configIndex][1];
			ripsMsg->assist2_nodeid = config[configIndex][2];
			ripsMsg->assist1_tuning = config[configIndex][3];
			ripsMsg->assist2_tuning = config[configIndex][4];

			if( ++configIndex >= CONFIGURATIONS )
				configIndex = 0;

			error = call AMSend.send(AM_BROADCAST_ADDR, &txMsg, sizeof(rips_start_message_t));
			ASSERT( error == SUCCESS );
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		ASSERT( error == SUCCESS );
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len)
	{
		return msg;
	}
}
