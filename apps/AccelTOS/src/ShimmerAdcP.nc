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

#include "Assert.h"
#include "ShimmerAdc.h"

//#define DEBUGCHN

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
		interface Alarm<T32khz,uint16_t> as Alarm;
		interface DiagMsg;
	}
}

implementation
{
	enum
	{
		SAMPLE_COUNT = 16,	// the maximum number of channels
		QUEUESIZE    = 100
	};

	uint16_t dt = 160; // 204.8 Hz
	
	// All norace variables are guarded by sampling
	bool sampling;

	norace uint8_t nSamples;

	uint8_t head;
	norace uint8_t tail;
	norace uint8_t step;
	norace uint8_t offset;
	norace uint8_t ts_offset;
	norace uint8_t cnt_offset;
	norace uint8_t size;
	norace uint8_t effSize;
	
	norace uint16_t queue[QUEUESIZE];

	norace bool leadingZerosNeeded;
	norace bool timestampNeeded;
	norace bool counterNeeded;
	norace uint16_t counter;
	
	void dumpInt(char* msg, uint16_t i) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.int16(i);
			call DiagMsg.send();
		}		
	}
	
	void dump(char* msg) {
		if( call DiagMsg.record() ) {
			call DiagMsg.str(msg);
			call DiagMsg.send();
		}		
	}
	
	void resetVChannels() {
		counter = 0;
		leadingZerosNeeded = FALSE;
		timestampNeeded    = FALSE;
		counterNeeded      = FALSE;
	}
	
	void resetQueue() {
		uint8_t i;
		head = 0;
		tail = 0;
		size = 0;
		step = 0;
		offset = 0;
		ts_offset = 0;
		cnt_offset = 0;
		for (i=0; i<QUEUESIZE; ++i) {
			queue[i] = 0;
		}
	}
	
	uint8_t setupVirtualChannels(uint8_t* channels, uint8_t count) {
		uint8_t i, retVal, realChannels;

		for (i=0, realChannels=0; i<count; ++i) {
			if (channels[i] == SHIMMER_ADC_ZERO) {
				if (!leadingZerosNeeded) {
					leadingZerosNeeded = TRUE;
					++step;
					++ts_offset;
					++cnt_offset;
				}
				else {
					ASSERT(FALSE); 
				}
			}
			else if (channels[i] == SHIMMER_ADC_TIME) {
				if (!timestampNeeded) {
					timestampNeeded = TRUE;
					++step; // Hideous but incremented once more later
					cnt_offset+=2;
				}
				else {
					ASSERT(FALSE); 
				}
			}
			else if (channels[i] == SHIMMER_ADC_COUNTER) {
				if (!counterNeeded) {
					counterNeeded = TRUE;
					++step;
				}
				else {
					ASSERT(FALSE); 
				}  
			}
			else {
				 ++realChannels; 
			}
		}

		retVal = count-step;
		ASSERT(retVal == realChannels);
		
		if (timestampNeeded)
			++step; // Hideous, timestamp is uint32_t
		
		offset = step;
		
		step += realChannels;
		
		ASSERT((QUEUESIZE>=step)&&(step>0));
		effSize = QUEUESIZE - (QUEUESIZE%step);
		
/*		dumpInt("step",     step);
		dumpInt("tsOffset", ts_offset);
		dumpInt("cntOffset",cnt_offset);
		dumpInt("offset",   offset);
		dumpInt("realChn",  realChannels); */
		dumpInt("effSize",  effSize);

		return retVal;
	}
	
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
			adc12tovie:0,		// conversion-time-overflow-interrupt: 0 = interrupt dissabled
			adc12ovie:0,		// ADC12MEMx overflow-interrupt: 0 = dissabled
			adc12on:1,			// ADC12 on: 1 = on
			refon:1,			// reference generator: 1 = on
			r2_5v:1,			// reference generator voltage: 1 = 2.5V
			msc:1,				// multiple sample and conversion: 1 = conversions performed ASAP
			sht0:SAMPLE_HOLD_4_CYCLES,	// sample-and-hold-time for ADC12MEM0 to ADC12MEM7
			sht1:SAMPLE_HOLD_4_CYCLES	// sample-and-hold-time for ADC12MEM8 to ADC12MEM15
		};     

		adc12ctl1_t ctl1 = {
			adc12busy:0,				// no operation is active
			conseq:1,					// conversion mode: sequence of chans
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
		
		bool recording = FALSE;
		int8_t i, j, nVirtChn;

		if( count > SAMPLE_COUNT )
			return ESIZE;
		
		atomic {
			if( sampling )
				return EBUSY;
			else 
				call Alarm.stop();
		}
		call HplAdc12.stopConversion();
		call Msp430DmaChannel.stopTransfer();
		// TODO At this point no transferDone is possible?
		resetVChannels();
		resetQueue();
		nSamples = setupVirtualChannels(channels, count);
		size+=step;
		
		call Msp430DmaChannel.setupTransfer(
			DMA_BLOCK_TRANSFER,		// copy all samples at once
			DMA_TRIGGER_ADC12IFGx,		// triggered by ADC12
			DMA_EDGE_SENSITIVE,		// edge sensitive trigger
			(void*)ADC12MEM0_,		// copy form ADC12 memory registers
			queue+offset,		// buffer
			nSamples,			// number of samples
			DMA_WORD,			// each sample is a word
			DMA_WORD,			// store the full word
			DMA_ADDRESS_INCREMENTED,	// auto increment src address 
			DMA_ADDRESS_INCREMENTED);	// auto increment dst address

		call HplAdc12.setCtl0(ctl0);
		call HplAdc12.setCtl1(ctl1);

		//recording = call DiagMsg.record();
		nVirtChn = count-nSamples;
		for (i=nVirtChn; i<count; ++i) {
			
			j = channels[i];
			
			if (j>15)
				continue;
			
/*			if(recording) {
				call DiagMsg.uint16(i-nVirtChn);
				call DiagMsg.uint16(j);
			}
*/
			memctl.inch = j;
			memctl.eos = 0;
			if (i==count-1)
				memctl.eos = 1;

			call HplAdc12.setMCtl(i-nVirtChn, memctl);

		}

/*		if (recording) {
			call DiagMsg.send();
		}
*/		// get ready for the first transfer
		call Msp430DmaChannel.startTransfer();

		return SUCCESS;
	}

	command error_t ShimmerAdc.sample(uint16_t period) {
	  
		error_t error = SUCCESS;

		atomic {
			if( sampling ) {
				error = EBUSY;
			}
			else {
				dt = period;
				call Alarm.start(dt);
			}
		}
		
		return error;
	}

	async event void Alarm.fired() {
		
		atomic {
			if (sampling) {
				dump("StillSampling");
			}
			else if (size>=effSize) {// FIXME Should be just == ?
				dump("BufferFull");				
			}
			else {
				sampling = TRUE;
				call HplAdc12.startConversion();
			}
			call Alarm.startAt(call Alarm.getAlarm(), dt);
		}
	}
	
	// we trigger the DMA
	async event void HplAdc12.conversionDone(uint16_t iv) { }
	
	task void reportDone() {
	  
		// FIXME size > 1?
	  	while (size > step) {
			signal ShimmerAdc.sampleDone(queue+head, step);
			
			head += step;
			if (head == effSize)
				head = 0;
				
			atomic {
				size-=step;
			}
		}
		//dumpInt("size", size);
	}
	
	async event void Msp430DmaChannel.transferDone(error_t success)
	{
#ifdef DEBUGCHN
		uint8_t i;
#endif
		if (timestampNeeded)
			*((uint32_t*) (queue+tail+ts_offset)) = call LocalTime.get();

		if (leadingZerosNeeded)
			ASSERT(queue[tail]==0); // resetQueue() took care of it

		if (counterNeeded)
			queue[tail+cnt_offset] = ++counter;
			
#ifdef DEBUGCHN
		for (i=0; i<nSamples; ++i)
			queue[tail+offset+i] = i+1;
#endif

		// prepare for the next ADC sample
		if (size < effSize) {
			tail += step;
			if (tail == effSize)
				tail = 0;
			size+=step;
		}
		else {
			ASSERT(FALSE);
		}
		call Msp430DmaChannel.repeatTransfer((void*)ADC12MEM0_, queue+tail+offset, nSamples);
		
		post reportDone();
		
		sampling = FALSE;
		// TODO Can the last two lines be swapped?
	}

	task void shutDown() {

		bool finished = FALSE;
		
		atomic {
			if(sampling) {
				post shutDown();
			}
			else {
				call Alarm.stop();
				finished = TRUE;
			}
		}
		
		if (finished) {
			signal ShimmerAdc.stopDone();
			// FIXME Stop conversion and transfer; Post reportDone?
			// call HplAdc12.stopConversion();
			// call Msp430DmaChannel.stopTransfer();
		}
	}

	command void ShimmerAdc.stop(){

		post shutDown();
	}
}
