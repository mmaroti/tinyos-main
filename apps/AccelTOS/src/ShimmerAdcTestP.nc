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

module ShimmerAdcTestP
{
	provides 
	{
		interface ShimmerAdc;
		interface Init;
	}

	uses
	{
		interface LocalTime<T32khz>;
	}
}

implementation
{
	enum
	{
		SAMPLE_COUNT = 16,	// the maximum number of channels
	};

	bool sampling;
	uint8_t channelCount;

	typedef struct buffer_t
	{
		uint32_t timestamp;
		uint16_t samples[16];
	} buffer_t;

	norace buffer_t buffer;

	command error_t Init.init()
	{
		return SUCCESS;
	}

	command error_t ShimmerAdc.setChannels(uint8_t *channels, uint8_t count)
	{
		if( count > SAMPLE_COUNT )
			return ESIZE;
		if( sampling )
			return EBUSY;

		channelCount = count;

		return SUCCESS;
	}

	task void reportDone()
	{
		sampling = FALSE;
		signal ShimmerAdc.sampleDone(buffer.timestamp, buffer.samples);
	}

	uint16_t counter;

	command error_t ShimmerAdc.sample()
	{
		uint8_t i;

		if( sampling )
			return FAIL;

		sampling = TRUE;

		buffer.timestamp = call LocalTime.get();
		buffer.samples[0] = ++counter;
		for(i = 1; i < channelCount; ++i)
			buffer.samples[i] = i;

		post reportDone();

		return SUCCESS;
	}
}
