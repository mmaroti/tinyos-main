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
	}

	event void SendTimer.fired()
	{
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len)
	{
		return msg;
	}
}
