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
* Author: Miklos Maroti
*/

#include <AM.h>

module BufferedSendP
{
	provides
	{
		interface BufferedSend;
	}

	uses
	{
		interface AMSend;
		interface Packet;
	}
}

implementation
{
	enum
	{
		BUFFER_SIZE = 2,
		ADDRESS = AM_BROADCAST_ADDR,
	};

	message_t messages[BUFFER_SIZE];

	uint8_t current;	// the currently recorded message buffer
	uint8_t position;	// the write position in the current buffer
	uint8_t pending;	// the number of full messages
	bool sending;

	task void sendMessage()
	{
		if( ! sending && pending > 0 )
		{
			int8_t first = current - pending;
			if( first < 0 )
				first += BUFFER_SIZE;

			if( call AMSend.send(ADDRESS, messages + first, call Packet.payloadLength(messages + first)) == SUCCESS )
				sending = TRUE;
			else
				post sendMessage();
		}
	}

	command error_t BufferedSend.send(void *data, uint8_t length)
	{
		if( pending >= BUFFER_SIZE )
			return FAIL;

		if( position + length > TOSH_DATA_LENGTH )
			call BufferedSend.flush();

		if( pending >= BUFFER_SIZE )
			return FAIL;

		memcpy(messages[current].data + position, data, length);
		position += length;

		return SUCCESS;
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		sending = FALSE;
	
		if( error == SUCCESS )
			--pending;
		else
			post sendMessage();
	}

	command void BufferedSend.flush()
	{
		if( position > 0 )
		{
			// store the length
			call Packet.setPayloadLength(messages + current, position);

			position = 0;
			if( ++current >= BUFFER_SIZE )
				current = 0;

			++pending;
			post sendMessage();
		}
	}
}
