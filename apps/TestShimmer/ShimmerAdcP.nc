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

module ShimmerAdcP
{
	provides 
	{
		interface ShimmerAdc;
		interface Init;
	}

	uses
	{
		interface HplAdc12;
		interface Msp430DmaControl;
		interface Msp430DmaChannel;
		interface LocalTime<T32khz>;
	}
}

implementation
{
	enum
	{
		STATE_OFF = 0,
		STATE_READY = 1,
		STATE_SAMPLING = 2,

		SAMPLE_COUNT = 16,	// the maximum number of channels
		BUFFER_COUNT = 2,	// number of recording buffers
	};

	norace uint8_t state;
	norace uint8_t channelCount;

	typedef struct buffer_t
	{
		uint32_t timestamp;
		uint16_t samples[SAMPLE_COUNT];
	} buffer_t;

	norace buffer_t buffers[BUFFER_COUNT];

	norace uint8_t current;	// the index of the current buffer
	uint8_t pending;	// the number of pending buffers to be reported

	command error_t Init.init()
	{
		call HplAdc12.setIEFlags(0);
		call HplAdc12.resetIFGs();

		call Msp430DmaControl.init();
		call Msp430DmaControl.setFlags(FALSE, FALSE, FALSE);  // sets DMACTL1

		return SUCCESS;
	}

	command error_t ShimmerAdc.setChannels(uint8_t *channels, uint8_t count)
	{
		adc12ctl0_t ctl0 = {
			adc12sc:0,			// start conversion: 0 = no sample-and-conversion-start
			enc:0,				// enable conversion: 0 = ADC12 disabled
			adc12tovie:0,			// conversion-time-overflow-interrupt: 0 = interrupt dissabled
			adc12ovie:0,			// ADC12MEMx overflow-interrupt: 0 = dissabled
			adc12on:1,			// ADC12 on: 1 = on
			refon:1,			// reference generator: 1 = on
			r2_5v:1,			// reference generator voltage: 1 = 2.5V
			msc:1,				// multiple sample and conversion: 1 = conversions performed ASAP
			sht0:SAMPLE_HOLD_4_CYCLES,	// sample-and-hold-time for ADC12MEM0 to ADC12MEM7
			sht1:SAMPLE_HOLD_4_CYCLES	// sample-and-hold-time for ADC12MEM8 to ADC12MEM15
		};     

		adc12ctl1_t ctl1 = {
			adc12busy:0,			// no operation is active
			conseq:1,			// conversion mode: sequence of chans
			adc12ssel:SHT_SOURCE_SMCLK,	// SHT_SOURCE_SMCLK=3; ADC12 clocl source
			adc12div:SHT_CLOCK_DIV_8,	// SHT_CLOCK_DIV_8=7; ADC12 clock div 1
			issh:0,				// sample-input signal not inverted
			shp:1,				// Sample-and-hold pulse-mode select: SAMPCON signal is sourced from the sampling timer
			shs:0,				// Sample-and-hold source select= ADC12SC bit
			cstartadd:0			// conversion start addres ADC12MEM0
		};

		adc12memctl_t memctl = {
			inch:0,				// input channel: ADC0
			sref:REFERENCE_VREFplus_AVss,	// stabilized reference voltage
			eos:1				// end of sequence flag: 1 indicates last conversion
		};

		if( count > SAMPLE_COUNT )
			return ESIZE;
		if( state == STATE_SAMPLING )
			return EBUSY;

		channelCount = count;

		call HplAdc12.stopConversion();
		call Msp430DmaChannel.stopTransfer();

		call Msp430DmaChannel.setupTransfer(
			DMA_BLOCK_TRANSFER,		// copy all samples at once
			DMA_TRIGGER_ADC12IFGx,		// triggered by ADC12
			DMA_EDGE_SENSITIVE,		// edge sensitive trigger
			(void*)ADC12MEM0_,		// copy form ADC12 memory regisers
			(void*)buffers[current].samples,// first buffer
			count,				// number of samples
			DMA_WORD,			// each sample is a word
			DMA_WORD,			// store the full word
			DMA_ADDRESS_INCREMENTED,	// auto increment src address 
			DMA_ADDRESS_INCREMENTED);	// auto increment dst address

		call HplAdc12.setCtl0(ctl0);
		call HplAdc12.setCtl1(ctl1);

		while( count > 0 )
		{
			--count;

			memctl.inch = channels[count];
			call HplAdc12.setMCtl(count, memctl);

			// the end of sequence is cleared for all others
			memctl.eos = 0;
		}

		// get ready for the first transfer
		call Msp430DmaChannel.startTransfer();

		state = STATE_READY;
		return SUCCESS;
	}

	// we trigger the DMA
	async event void HplAdc12.conversionDone(uint16_t iv) { }

	command error_t ShimmerAdc.sample()
	{
		if( state != STATE_READY )
			return FAIL;

		state |= STATE_SAMPLING;
		call HplAdc12.startConversion();

		return SUCCESS;
	}

	task void reportDone();

	async event void Msp430DmaChannel.transferDone(error_t success)
	{
		buffers[current].timestamp = call LocalTime.get();

		atomic
		{
			++pending;
			if( ++current >= BUFFER_COUNT )
				current = 0;
		}

		call Msp430DmaChannel.repeatTransfer((void*)ADC12MEM0_, buffers[current].samples, channelCount);

		post reportDone();

		state = STATE_READY;
	}

	task void reportDone()
	{
		int8_t report;

		atomic
		{
			report = current - pending;
			--pending;
		}

		if( report < 0 )
			report += BUFFER_COUNT;

		signal ShimmerAdc.sampleDone(buffers[report].timestamp, buffers[report].samples);		
	}
}
