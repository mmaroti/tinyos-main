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

#include "Serial.h"
#include "SerialDebug.h"

module SerialFrameP
{
	provides
	{
		interface SerialComm as SerialSend;
	}

	uses
	{
		interface SerialComm as SubSend;
	}
}

implementation
{
// ------- Send	

	enum
	{
		TXSTATE_START = 0,
		TXSTATE_DATA = 1,
		TXSTATE_ESCAPED = 2,
		TXSTATE_STOP = 3,
	};

	norace uint8_t txState;
	norace uint8_t txByte;

	async command error_t SerialSend.start()
	{
		SERIAL_ASSERT( txState == TXSTATE_START );

		return call SubSend.start();
	}

	async event void SubSend.startDone()
	{
		SERIAL_ASSERT( txState == TXSTATE_START );

		call SubSend.send(HDLC_FLAG_BYTE);
	}

	async command void SerialSend.send(uint8_t byte)
	{
		SERIAL_ASSERT( txState == TXSTATE_DATA );

		// make fast path fall through
		if( byte != HDLC_FLAG_BYTE && byte != HDLC_CTLESC_BYTE )
			call SubSend.send(byte);
		else
		{
			txByte = byte ^ 0x20;
			txState = TXSTATE_ESCAPED;
			call SubSend.send(HDLC_CTLESC_BYTE);
		}
	}

	async event void SubSend.sendDone()
	{
		// make fast path fall through
		if( txState == TXSTATE_DATA )
			signal SerialSend.sendDone();
		else if( txState == TXSTATE_ESCAPED )
		{
			txState = TXSTATE_DATA;
			call SubSend.send(txByte);
		}
		else if( txState == TXSTATE_START )
		{
			txState = TXSTATE_DATA;
			signal SerialSend.startDone();
		}
		else
		{
			SERIAL_ASSERT( txState == TXSTATE_STOP );
			call SubSend.stop();
		}
	}

	async command void SerialSend.stop()
	{
		SERIAL_ASSERT( txState == TXSTATE_DATA );

		txState = TXSTATE_STOP;
		call SubSend.send(HDLC_FLAG_BYTE);
	}

	async event void SubSend.stopDone(error_t error)
	{
		SERIAL_ASSERT( txState == TXSTATE_STOP );
		txState = TXSTATE_START;

		signal SerialSend.stopDone(error);
	}

// ------- Receive
}
