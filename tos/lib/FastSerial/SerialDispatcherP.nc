/** Copyright (c) 2010, University of Szeged
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

#include "SerialDebug.h"

module SerialDispatcherP
{
	provides
	{
		interface SplitControl;
		interface Receive[uart_id_t];
		interface Send[uart_id_t];
	}

	uses
	{
		interface StdControl as SubControl;
		interface SerialComm as SubSend;
		interface SerialPacketInfo[uart_id_t id];
	}
}

implementation
{
	enum
	{
		STATE_OFF = 0,
		STATE_OFF_TO_ON = 1,
		STATE_ON_TO_OFF = 2,
		STATE_ON = 3,
		STATE_SEND = 4,
	};

	norace uint8_t state;

	task void signalDone();

// ------- SplitControl

	command error_t SplitControl.start()
	{
		error_t error;

		if( state != STATE_OFF )
			return EALREADY;

		error = call SubControl.start();
		if( error != SUCCESS )
			return error;

		state = STATE_OFF_TO_ON;
		post signalDone();
		return SUCCESS;
	}

	command error_t SplitControl.stop()
	{
		error_t error;

		if( state != STATE_ON )
			return EALREADY;

		error = call SubControl.stop();
		if( error != SUCCESS )
			return error;

		state = STATE_ON_TO_OFF;
		post signalDone();
		return SUCCESS;
	}

	default event void SplitControl.startDone(error_t err) { }
	default event void SplitControl.stopDone(error_t err) { }

// ------- Send

	message_t *txMsg;
	norace uint8_t *txPtr;
	norace uint8_t *txEnd;
	norace uint8_t txId;
	norace uint8_t txError;

	command error_t Send.send[uart_id_t id](message_t* msg, uint8_t len)
	{
		if( state != STATE_ON )
			return EOFF;

		txMsg = msg;
		txPtr = ((uint8_t*)msg) + call SerialPacketInfo.offset[id]();
		txEnd = txPtr + call SerialPacketInfo.dataLinkLength[id](msg, len);
		txId = id;

		return call SubSend.start();
	}

	async event void SubSend.startDone()
	{
		SERIAL_ASSERT( state == STATE_ON );

		state = STATE_SEND;
		call SubSend.send(txId);
	}
	
	async event void SubSend.sendDone()
	{
		SERIAL_ASSERT( state == STATE_SEND );

		if( txPtr != txEnd )
			call SubSend.send( *(txPtr++) );
		else
			call SubSend.stop();
	}

	async event void SubSend.stopDone(error_t error)
	{
		SERIAL_ASSERT( state == STATE_SEND );

		txError = error;
		post signalDone();
	}

	command error_t Send.cancel[uart_id_t id](message_t *msg)
	{
		return FAIL;
	}
	
	command uint8_t Send.maxPayloadLength[uart_id_t id]()
	{
		return sizeof(message_header_t) + TOSH_DATA_LENGTH + sizeof(message_footer_t) - call SerialPacketInfo.offset[id]();
	}

	command void* Send.getPayload[uart_id_t id](message_t* msg, uint8_t len)
	{
		if( len > call Send.maxPayloadLength[id]() )
			return NULL;
		else
			return ((void*)msg) + call SerialPacketInfo.offset[id]();
	}

	default event void Send.sendDone[uart_id_t id](message_t *msg, error_t error)
	{
	}

// ------- Receive

// ------- SignalDone

	task void signalDone()
	{
		if( state == STATE_OFF_TO_ON )
		{
			state = STATE_ON;
			signal SplitControl.startDone(SUCCESS);
		}
		else if( state == STATE_ON_TO_OFF )
		{
			state = STATE_OFF;
			signal SplitControl.stopDone(SUCCESS);
		}
		else if( state == STATE_SEND )
		{
			state = STATE_ON;
			signal Send.sendDone[txId](txMsg, txError);
		}
	}

	default async command uint8_t SerialPacketInfo.offset[uart_id_t id]()
	{
		return 0;
	}

	default async command uint8_t SerialPacketInfo.dataLinkLength[uart_id_t id](message_t *msg, uint8_t len)
	{
		return 0;
	}
}
