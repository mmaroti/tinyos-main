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

#ifndef __HPLATM1281TIMER_H__
#define __HPLATM1281TIMER_H__

// ----- 8-bit timer control register (TCCR), clock select bits (CS)

enum
{
	ATM1281_CLK8_OFF = 0,
	ATM1281_CLK8_NORMAL,
	ATM1281_CLK8_DIVIDE_8,
	ATM1281_CLK8_DIVIDE_32,
	ATM1281_CLK8_DIVIDE_64,
	ATM1281_CLK8_DIVIDE_128,
	ATM1281_CLK8_DIVIDE_256,
	ATM1281_CLK8_DIVIDE_1024,
};

// ----- 8-bit timer control register (TCCR), waveform generation mode (WGM)

enum
{
	ATM1281_WAVE8_NORMAL = 0,
	ATM1281_WAVE8_PWM,
	ATM1281_WAVE8_CTC,
	ATM1281_WAVE8_PWM_FAST,
};

// extra flags for Timer.setMode() for the asynchronous timer
enum
{
	ATM1281_ASYNC_OFF = 0x00,
	ATM1281_ASYNC_ON  = 0x20,	// ASSR register AS2 bit
	AYM1281_ASYNC_EXT = 0x60,	// ASSR register EXCLK bit
};

// ----- timer control register (TCCR), compare output mode (COM)

enum
{
	ATM1281_COMPARE8_OFF = 0, 
	ATM1281_COMPARE8_TOGGLE,
	ATM1281_COMPARE8_CLEAR,
	ATM1281_COMPARE8_SET,
};

// ----- 16-bit timer control register (TCCR), clock select bits (CS)

enum
{
	ATM1281_CLK16_OFF = 0,
	ATM1281_CLK16_NORMAL,
	ATM1281_CLK16_DIVIDE_8,
	ATM1281_CLK16_DIVIDE_64,
	ATM1281_CLK16_DIVIDE_256,
	ATM1281_CLK16_DIVIDE_1024,
	ATM1281_CLK16_EXTERNAL_FALL,
	ATM1281_CLK16_EXTERNAL_RISE,
};

// ----- 16-bit timer control register (TCCR), waveform generation mode (WGM)

enum
{
	ATM1281_WAVE16_NORMAL = 0,
	ATM1281_WAVE16_PWM_8BIT,
	ATM1281_WAVE16_PWM_9BIT,
	ATM1281_WAVE16_PWM_10BIT,
	ATM1281_WAVE16_CTC_COMPARE,
	ATM1281_WAVE16_PWM_FAST_8BIT,
	ATM1281_WAVE16_PWM_FAST_9BIT,
	ATM1281_WAVE16_PWM_FAST_10BIT,
	ATM1281_WAVE16_PWM_CAPTURE_LOW,
	ATM1281_WAVE16_PWM_COMPARE_LOW,
	ATM1281_WAVE16_PWM_CAPTURE_HIGH,
	ATM1281_WAVE16_PWM_COMPARE_HIGH,
	ATM1281_WAVE16_CTC_CAPTURE,
	ATM1281_WAVE16_RESERVED,
	ATM1281_WAVE16_PWM_FAST_CAPTURE,
	ATM1281_WAVE16_PWM_FAST_COMPARE,
};

// ----- 16-bit timer control register (TCCR), compare output mode (COM)

enum
{
	ATM1281_COMPARE16_NORMAL = 0,
	ATM1281_COMPARE16_TOGGLE,
	ATM1281_COMPARE16_CLEAR,
	ATM1281_COMPARE16_SET
};

#endif//__HPLATM1281TIMER_H__
