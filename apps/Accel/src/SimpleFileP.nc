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

module SimpleFileP
{
	uses
	{
		interface Leds;
		interface SD;
		interface StdControl as SDControl;
	}

	// TODO A deep format functionality?
	provides
	{
		interface SplitControl;
		interface SimpleFile;
	}
}

// TODO Document that Led0 ON indicates error
implementation
{
	// TODO Probably a bool would suffice and executeCommand could be eliminated
	enum
	{
		STATE_OFF = 0,
		STATE_BOOTING = 1,
		STATE_READY = 2,
		STATE_FORMAT = 3,
		STATE_READ = 4,
		STATE_WRITE = 5,
	};

	uint8_t state = STATE_OFF;
	// FIXME Only read once; plus we got available==true with SD card out!
	bool available = FALSE;

	struct buffer
	{
		uint16_t length;
		uint8_t data[510]; // TODO Eliminate magic number 510
	} buffer;

	uint32_t cardSize;
	uint32_t writePos;
	uint32_t readPos;

	// these are set by the user
	uint8_t *packetPtr;
	uint16_t packetLen;

	task void executeCommand();

	async event void SD.available()
	{
		available = TRUE;
	}

	async event void SD.unavailable()
	{
		available = FALSE;
	}

	void findLastSector()
	{				
		error_t error;

		if( ! available )
			post executeCommand(); // TODO What does this mean?

		cardSize = call SD.readCardSize();
		readPos = 0;

		for(writePos = 0; writePos < cardSize; ++writePos)
		{
			// TODO The cast assumes a specific memory layout of buffer - guaranteed?
			error = call SD.readBlock(writePos, (uint8_t*) &buffer);

			if( error != SUCCESS )
				break;

			if( buffer.length == 0 ) {
				state = STATE_READY;
				signal SplitControl.startDone(SUCCESS);
				return;
			}
		}

		// Error: either card is full or a readBlock failed
		state = STATE_OFF;
		call Leds.led0On();		

		// ignore the error, hope it stops properly
		call SDControl.stop();

		signal SplitControl.startDone(FAIL);
	}

	command error_t SplitControl.start()
	{
		error_t error;

		if( state != STATE_OFF )
			return FAIL;

		error = call SDControl.start();
		if( error != SUCCESS )
			return error;

		state = STATE_BOOTING;
		post executeCommand();
		return SUCCESS;
	}

	command error_t SplitControl.stop()
	{
		error_t error;

		if( state != STATE_READY )
			return EBUSY;

		// ignore the error, it is not clear what to do if this fails
		error = call SDControl.stop();
		if( error != SUCCESS )
			call Leds.led0On();

		state = STATE_OFF;
		// TODO Return variable error instead?
		return SUCCESS;
	}

	void formatDevice()
	{
		error_t error;

		buffer.length = 0;
		error = call SD.writeBlock(0, (uint8_t*) &buffer);

		if( error == SUCCESS )
		{
			writePos = 0;
			readPos = 0;
		}
		
		state = STATE_READY; // TODO What if write fails (not EBUSY)?
		signal SimpleFile.formatDone(error);
	}

	command error_t SimpleFile.format()
	{
		if( state != STATE_READY )
			return EBUSY;  // TODO Sure? What if STATE_OFF for example?

		state = STATE_FORMAT;
		post executeCommand();

		return SUCCESS;
	}

	void readSector()
	{
		error_t error;
		uint16_t i;

		error = call SD.readBlock(readPos, (uint8_t*) &buffer);
		if( error == SUCCESS )
		{
			if( packetLen > buffer.length )
				packetLen = buffer.length;

			for(i = 0; i < packetLen; ++i)
				packetPtr[i] = buffer.data[i];

			++readPos;	// FIXME readPos < writePos not checked
		}
		else
			packetLen = 0;

		state = STATE_READY;
		signal SimpleFile.readDone(error, packetLen);
	}

	command error_t SimpleFile.read(uint8_t *packet, uint16_t length)
	{
		if( state != STATE_READY )
			return EBUSY;

		packetPtr = packet;
		// FIXME Check if length is <= 510?
		packetLen = length;
		state = STATE_READ;
		// FIXME The line below was missing?
		post executeCommand();
		return SUCCESS;
	}

	void writeSector()
	{
		error_t error;
		uint16_t i;

		for(i = 0; i < packetLen; ++i)
			buffer.data[i] = packetPtr[i];

		buffer.length = packetLen;

		error = call SD.writeBlock(writePos, (uint8_t*) &buffer);
		if( error == SUCCESS )
			++writePos; // FIXME writePos < cardSize

		state = STATE_READY;
		signal SimpleFile.appendDone(error);
	}

	command error_t SimpleFile.append(uint8_t *packet, uint16_t length)
	{
		if( state != STATE_READY )
			return EBUSY;
		else if( length > 510 )
			return ESIZE;

		packetPtr = packet;
		packetLen = length;
		state = STATE_WRITE;
		return SUCCESS;
	}

	command error_t SimpleFile.seek(uint32_t index)
	{
		if( index < writePos )
		{
			readPos = index;
			return SUCCESS;
		}
		else
			return FAIL; // TODO Return a more specific error value?
	}

	command uint32_t SimpleFile.size()
	{
		return writePos;
	}

	// TODO Please explain the benefit of this approach
	task void executeCommand()
	{
		if     ( state == STATE_BOOTING )
			findLastSector();
		else if( state == STATE_FORMAT )
			formatDevice();
		else if( state == STATE_READ )
			readSector();
		else if( state == STATE_WRITE )
			writeSector();
		else
			call Leds.led0On();
	}
}
