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

	provides
	{
		interface SplitControl;
		interface SimpleFile;
	}
}

implementation
{
	void SIMPLE_ASSERT(bool condition)
	{
		if( ! condition )
			call Leds.led0On();
	}

	enum
	{
		STATE_OFF = 0,
		STATE_BOOTING = 1,
		STATE_READY = 2,
	};

	uint8_t state = STATE_OFF;
	norace bool available = FALSE;

	/*struct buffer
	{
		uint16_t length;
		uint8_t data[510];
	} buffer;*/

	uint32_t cardSize;
	uint32_t position;

	task void findLastSector()
	{
		//uint32_t index;
		uint8_t buffer[512];

		SIMPLE_ASSERT( state == STATE_BOOTING );

		if( ! available )
			post findLastSector();

		cardSize = call SD.readCardSize();

		for(position = 0; position < cardSize; ++position)
		{
			error_t error = call SD.readBlock(position, buffer);
			if( error != SUCCESS )
			{
				state = STATE_OFF;
				call SDControl.stop();
				signal SplitControl.startDone(FAIL);
				// TODO Should not we exit here?
			}

			if(buffer[0] == 0 && buffer[1] == 0)
				break;
		}
		
		// FIXME What if card is full?

		state = STATE_READY;
		signal SplitControl.startDone(SUCCESS);
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

		post findLastSector();
	}

	async event void SD.available()
	{
		available = TRUE;
	}

	async event void SD.unavailable()
	{
		available = FALSE;
	}

	command error_t SplitControl.stop(){
		// TODO Auto-generated method stub
		return 1;
	}

	command error_t SimpleFile.read(uint8_t *packet, uint16_t length){
		// TODO Auto-generated method stub
		return 1;
	}

	command error_t SimpleFile.append(uint8_t *packet, uint16_t length){
		// TODO Auto-generated method stub
		return 1;
	}

	command error_t SimpleFile.seek(uint32_t index){
		// TODO Auto-generated method stub
		return 1;
	}

	command uint32_t SimpleFile.size(){
		// TODO Auto-generated method stub
		return 1;
	}

	command error_t SimpleFile.format(){
		// TODO Auto-generated method stub
		return 1;
	}
}
