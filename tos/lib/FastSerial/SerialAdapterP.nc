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

module SerialAdapterP
{
	provides
	{
		interface SerialComm as SerialSend;
	}

	uses
	{
		interface UartStream;
	}
}

implementation
{
// ------- Send	

	enum
	{
		TXSTATE_OFF = 0,
		TXSTATE_STARTED = 0x01,
		TXSTATE_ERROR = 0x02,
		TXSTATE_SENDDONE = 0x04,
		TXSTATE_PENDING = 0x08,
		TXSTATE_STARTDONE = 0x10,
		TXSTATE_STOPDONE = 0x20,
	};

	norace uint8_t txState;
	norace uint8_t txByte;

	task void signalDone()
	{
		SERIAL_ASSERT( txState == (TXSTATE_STARTED | TXSTATE_STARTDONE) || txState == (TXSTATE_STARTED | TXSTATE_STOPDONE) 
			|| txState == (TXSTATE_STARTED | TXSTATE_STOPDONE | TXSTATE_ERROR) );

		if( (txState & TXSTATE_STARTDONE) != 0 )
		{
			txState &= ~TXSTATE_STARTDONE;
			signal SerialSend.startDone();
		}
		else if( (txState & TXSTATE_STOPDONE) != 0 )
		{
			error_t error = (txState & TXSTATE_ERROR) != 0 ? FAIL : SUCCESS;

			txState = TXSTATE_OFF;

			signal SerialSend.stopDone(error);
		}
	}

	async command error_t SerialSend.start()
	{
		SERIAL_ASSERT( txState == TXSTATE_OFF );

		txState = TXSTATE_STARTED | TXSTATE_STARTDONE;
		post signalDone();

		return SUCCESS;
	}

	async command void SerialSend.send(uint8_t byte)
	{
		SERIAL_ASSERT( (txState & TXSTATE_STARTED) != 0 && (txState & TXSTATE_PENDING) == 0 );

		txByte = byte;

		if( call UartStream.send( &txByte, 1 ) != SUCCESS )
			txState |= TXSTATE_ERROR;
	}

	async event void UartStream.sendDone(uint8_t* buf, uint16_t len, error_t error)
	{
		SERIAL_ASSERT( (txState & TXSTATE_STARTED) != 0 && (txState & TXSTATE_PENDING) == 0 );

		if( error != SUCCESS )
			txState |= TXSTATE_ERROR;

		if( (txState & TXSTATE_SENDDONE) == 0 )
		{
			txState |= TXSTATE_SENDDONE;

			// keep delivering sendDone events if the interrupt has occured again while it was executing
			do
			{
				signal SerialSend.sendDone();

				atomic
				{
					if( (txState & TXSTATE_PENDING) != 0 )
						txState &= ~TXSTATE_PENDING;
					else
						txState &= ~TXSTATE_SENDDONE;
				}
			} while( (txState & TXSTATE_SENDDONE) != 0 );
		}
		else // we have reentered twice
			txState |= TXSTATE_PENDING;
	}

	async command void SerialSend.stop()
	{
		SERIAL_ASSERT( (txState & TXSTATE_STARTED) != 0 && (txState & TXSTATE_PENDING) == 0 );

		// it is possible that TXSTATE_SENDDONE is on, so we have to signal stopDone from a task
		txState |= TXSTATE_STOPDONE;
		post signalDone();
	}

// ------- Receive

	async event void UartStream.receivedByte(uint8_t data)
	{
	}

	async event void UartStream.receiveDone(uint8_t* buf, uint16_t len, error_t error)
	{
	}
}
