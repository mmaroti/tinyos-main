/*
* Copyright (c) 2009, University of Szeged
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

#include "Bma180.h"

/*#ifdef ADC_DEBUG
	void assert(bool condition, const char* file, uint16_t line);
	#define ADC_ASSERT(COND) assert(COND, __FILE__, __LINE__)
#else
	#define ADC_ASSERT(COND) for(;0;)
#endif
*/
module BmaStreamP
{
  provides
  {
    interface ReadStream<bma180_data_t>;
  }

  uses
  {
    interface Read<bma180_data_t>;
    interface Atm128Calibrate;
    interface DiagMsg;
  }
}

implementation 
{
  enum
  {
    STATE_READY = 0,

    STATE_20 = 1,		// 2 buffers to be filled, 0 to be reported
    STATE_11 = 2,		// 1 buffer to be filled, 1 to be reported
    STATE_02 = 3,		// 0 buffer to be filled, 2 to be reported
    STATE_10 = 4,		// 1 buffer to be filled, 0 to be reported
    STATE_01 = 5,		// 0 buffer to be filled, 1 to be reported
    STATE_00 = 7,		// error reporting

    SAMPLING_STEP = 1,	// state increment after sampling
    REPORTING_STEP = 2,	// state increment after reporting
  };

  norace uint8_t state;

  bma180_data_t * firstStart;
  uint16_t firstLength;

  norace bma180_data_t * secondStart;
  norace uint16_t secondLength;

  // ------- Fast path

  norace bma180_data_t * currentPtr;
  norace bma180_data_t * currentEnd;

  task void bufferDone();

  event void Read.readDone(error_t err, bma180_data_t data)
  {
    //ADC_ASSERT( currentPtr != NULL && currentPtr < currentEnd );
    //ADC_ASSERT( state == STATE_20 || state == STATE_11 || state == STATE_10 );
    if(call DiagMsg.record()) {
      call DiagMsg.str("P_stream.readDone");
      call DiagMsg.send();
    }   
    *(currentPtr++) = data;

    if( currentPtr != currentEnd )
      return;

    currentPtr = secondStart;
    currentEnd = currentPtr + secondLength;

    if( (state += SAMPLING_STEP) != STATE_11 ) ;
      //call Atm128Adc.cancel();

    post bufferDone();
  }

	// ------- Slow path


  uint16_t actualPeriod;

  typedef struct free_buffer_t
  {
    uint16_t count;
    struct free_buffer_t * next;
  } free_buffer_t;

  free_buffer_t * freeBuffers;

  task void bufferDone()
  {
    uint8_t s;

    bma180_data_t * reportStart = firstStart;
    uint16_t reportLength = firstLength;
    if(call DiagMsg.record()) {
      call DiagMsg.str("P_bufferdonetask");
      call DiagMsg.send();
    }

    //ADC_ASSERT( state == STATE_11 || state == STATE_02 || state == STATE_01 || state == STATE_00 );

    firstStart = secondStart;
    firstLength = secondLength;

    atomic
    {
      s = state;

      if( s == STATE_11 && freeBuffers != NULL )
      {
        secondStart = (bma180_data_t *)freeBuffers;
        secondLength = freeBuffers->count;
        freeBuffers = freeBuffers->next;

        state = STATE_20;
      }
      else if( s != STATE_00 )
        state = s + REPORTING_STEP;
    }

    if( s != STATE_00 || freeBuffers != NULL )
    {
      if( s == STATE_00 )
      {
        reportStart = (bma180_data_t *)freeBuffers;
        reportLength = freeBuffers->count;
        freeBuffers = freeBuffers->next;
      }
      if(call DiagMsg.record()) {
        call DiagMsg.str("sig_buffdone");
        call DiagMsg.str(s!=STATE_00?"SUCCESS":"FAIL");
        call DiagMsg.send();
      }
      signal ReadStream.bufferDone(s != STATE_00 ? SUCCESS : FAIL, reportStart, reportLength);
    }

    if( freeBuffers == NULL && (s == STATE_00 || s == STATE_01) )
    {
      if(call DiagMsg.record()) {
        call DiagMsg.str("sig_readdone");
        call DiagMsg.send();
      }
      signal ReadStream.readDone(s == STATE_01 ? SUCCESS : FAIL, actualPeriod); 
      state = STATE_READY;
    }
    else if( s != STATE_11 )
      post bufferDone();
  }

  command error_t ReadStream.postBuffer(bma180_data_t *buffer, uint16_t count)
  {
    free_buffer_t * * last;

    if( count < (sizeof(free_buffer_t) + 1) >> 1 )
      return ESIZE;

    if(call DiagMsg.record()) {
      call DiagMsg.str("P_postbuff");
      call DiagMsg.send();
    }

    atomic
    {
      if( state == STATE_10 )
      {
        secondStart = buffer;
        secondLength = count;

        state = STATE_20;
        return SUCCESS;
      }
    }

    last = & freeBuffers;

    while( *last != NULL )
      last = &((*last)->next);
	
    *last = (free_buffer_t *)buffer;
    (*last)->count = count;
    (*last)->next = NULL;

    return SUCCESS;
  }

// TODO: define these next to PLATFORM_MHZ
#if defined(PLATFORM_IRIS) || defined(PLATFORM_MICAZ) || defined(PLATFORM_MICA2)
#define PLATFORM_HZ 7372800
#endif

#ifndef PLATFORM_HZ
#define PLATFORM_HZ (1000000 * PLATFORM_MHZ)
#endif

//#define PERIOD(prescaler) (uint16_t)(1000000.0 * 13 * prescaler / PLATFORM_HZ)

  command error_t ReadStream.read(uint32_t period)
  {
    if( state != STATE_READY )
      return EBUSY;

    if( freeBuffers == NULL )
      return FAIL;

    // do it early
    //call Read.read();

    firstStart = (bma180_data_t *)freeBuffers;
    firstLength = freeBuffers->count;
    freeBuffers = freeBuffers->next;

    currentPtr = firstStart;
    currentEnd = firstStart + firstLength;

    if( freeBuffers == NULL )
      state = STATE_10;
    else
    {
      secondStart = (bma180_data_t *)freeBuffers;
      secondLength = freeBuffers->count;
      freeBuffers = freeBuffers->next;

      state = STATE_20;
    }

    actualPeriod = period;	// TODO: correct for MHZ differences

		call Read.read();
    if(call DiagMsg.record()) {
      call DiagMsg.str("P_stream.Read");
      call DiagMsg.send();
    }   
    return SUCCESS;
  }

}
