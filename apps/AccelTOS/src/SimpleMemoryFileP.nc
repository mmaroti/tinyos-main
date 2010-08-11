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
* Author: Miklos Maroti, Ali Baharev
*/

// FIXME Code duplication!

module SimpleMemoryFileP
{
	uses
	{
		interface LedHandler;
	}

	// TODO A deep format functionality?
	provides
	{
		interface SplitControl;
		interface SimpleFile;
	}
}

implementation
{
	enum
	{
		END_OF_DATA = 127,
		BUFFSIZE = 506,
		SECTORS = 8,
	};

	struct buffer
	{
		uint16_t formatID; // ignored
		uint16_t moteID; 	// ignored
		uint16_t length;
		uint8_t data[BUFFSIZE];
	};

	struct buffer buffers[SECTORS];

	uint32_t writePos;
	uint32_t readPos;
	
	void format() {
		uint16_t i, j;
		uint8_t* data;
		readPos = 0;
		writePos = 0;
		for (i=0; i<SECTORS; ++i) {
			buffers[i].formatID = 0;
			buffers[i].length = 0;
			data = buffers[i].data;
			for (j=0; j<BUFFSIZE; ++j) {
				data[j] = 0;
			}
		}
	}

	task void startDone()
	{
		format(); // BUG writePos was not set to zero in the original implementation
		signal SplitControl.startDone(SUCCESS);
	}

	command error_t SplitControl.start()
	{
		post startDone();
		return SUCCESS;
	}

	task void stopDone()
	{
		signal SplitControl.stopDone(SUCCESS);
	}

	command error_t SplitControl.stop()
	{
		post stopDone();
		return SUCCESS;
	}

	task void formatDone()
	{
		format();
		signal SimpleFile.formatDone(SUCCESS);
	}

	command error_t SimpleFile.format()
	{
		post formatDone();
		return SUCCESS;
	}

	uint16_t readLength;

	task void readDone()
	{
		signal SimpleFile.readDone(SUCCESS, readLength);
	}

	command error_t SimpleFile.read(uint8_t *packet, uint16_t length)
	{
		uint16_t i;

		if( length > BUFFSIZE )
			return ESIZE;
		if( readPos >= writePos )
			return END_OF_DATA;

		if( length > buffers[readPos].length )
			length = buffers[readPos].length;

		// we need it in the readDone task
		readLength = length;

		for(i = 0; i < length; ++i)
			packet[i] = buffers[readPos].data[i];

		++readPos;

		post readDone();

		return SUCCESS;
	}

	task void appendDone()
	{
		signal SimpleFile.appendDone(SUCCESS);
	}

	command error_t SimpleFile.append(uint8_t *packet, uint16_t length)
	{
		uint16_t i;

		if( length > BUFFSIZE )
			return ESIZE;
		if( writePos >= SECTORS )
			return END_OF_DATA;

		buffers[writePos].length = length;

		for(i = 0; i < length; ++i)
			buffers[writePos].data[i] = packet[i];

		++writePos;
		post appendDone();

		return SUCCESS;
	}
	
	task void seekDone()
	{
		signal SimpleFile.seekDone(SUCCESS);
	}

	command error_t SimpleFile.seek(uint32_t pos)
	{
		if( pos < writePos )
		{
			readPos = pos;
			post seekDone();
			return SUCCESS;
		}
		else
			return FAIL;
	}

	command uint32_t SimpleFile.size()
	{
		return writePos;
	}
}
