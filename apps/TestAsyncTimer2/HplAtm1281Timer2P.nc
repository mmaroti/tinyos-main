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

#include "HplAtm1281Timer.h"

module HplAtm1281Timer2P @safe()
{
	provides
	{
		interface HplAtmTimer<uint8_t> as Timer;
		interface HplAtmCompare<uint8_t> as CompareA;
//		interface HplAtmCompare<uint8_t> as CompareB;
		interface McuPowerOverride;
	}

	uses
	{
		interface Leds;
		interface McuPowerState;
	}
}

implementation
{
/*
	Updating certain registers take 1-2 32768 KHz clock ticks if the timer 
	is running asynchronously, so we have to monitor when these updates are 
	propagated. We always check ASSR before updating these registers, and we 
	do not wait for completion after the change to make good use of the 
	processor time. However, we force the mcu power state calculation and 
	before entering power down mode we wait for the completion of these 
	register updates.
*/
// ----- TIMER: timer counter register (TCNT)

	async command uint8_t Timer.get()
	{
		atomic
		{
//			TCCR2A = TCCR2A;
//			while( ASSR & (1 << TCR2AUB) )
//				;

			return TCNT2;
		}
	}

	async command void Timer.set(uint8_t value)
	{
		atomic
		{
			while( ASSR & (1 << TCN2UB) )
				;

			TCNT2 = value;
		}

		call McuPowerState.update();
	}

// ----- TIMER: timer interrupt flag register (TIFR), timer overflow flag (TOV)

	default async event void Timer.overflow() { }

	AVR_ATOMIC_HANDLER(SIG_OVERFLOW2) { signal Timer.overflow(); }

	async command bool Timer.test() { return TIFR2 & (1 << TOV2); }

	async command void Timer.reset() { TIFR2 = 1 << TOV2; }

// ----- TIMER: timer interrupt mask register (TIMSK), timer overflow interrupt enable (TOIE)

	async command void Timer.start()
	{
		SET_BIT(TIMSK2, TOIE2);

		call McuPowerState.update();
	}

	async command void Timer.stop()
	{
		CLR_BIT(TIMSK2, TOIE2);

		call McuPowerState.update();
	}

	async command bool Timer.isOn() { return TIMSK2 & (1 << TOIE2); }

// ----- TIMER: timer control register (TCCR), clock select bits (CS)

	async command void Timer.setScale(uint8_t scale)
	{
		scale &= 0x07;

		atomic
		{
			while( ASSR & (1 << TCR2BUB) )
				;

			TCCR2B = (TCCR2B & 0xF8) | scale;
		}

		call McuPowerState.update();
	}

	async command uint8_t Timer.getScale() { return TCCR2B & 0x07; }

// ----- TIMER: timer control register (TCCR), waveform generation mode (WGM)

	async command void Timer.setMode(uint8_t mode)
	{
		atomic
		{
			while( ASSR & (1 << TCR2AUB | 1 << TCR2BUB) )
				;

			ASSR = (ASSR & 0x9F) | (mode & 0x60);
			TCCR2A = (TCCR2A & 0xFC) | (mode & 0x03);
			TCCR2B = (TCCR2B & 0xF7) | ((mode & 0x04) << 1);
		}

		call McuPowerState.update();
	}

	async command uint8_t Timer.getMode()
	{
		uint8_t a, b, c;

		atomic
		{
			a = ASSR;
			b = TCCR2A;
			c = TCCR2B;
		}

		return (a & 0x60) | (b & 0x03) | ((c >> 1) & 0x04);
	}

// ----- COMPAREA: output compare register (OCR)

	async command uint8_t CompareA.get() { return OCR2A; }

	async command void CompareA.set(uint8_t value)
	{
		atomic
		{
			while( ASSR & (1 << OCR2AUB) )
				;

			OCR2A = value;
		}

		call McuPowerState.update();
	}

// ----- COMPAREA: timer interrupt flag register (TIFR), output comare match flag (OCF)

	default async event void CompareA.fired() { }

	AVR_ATOMIC_HANDLER(SIG_OUTPUT_COMPARE2A) { signal CompareA.fired(); }

	async command bool CompareA.test() { return TIFR2 & (1 << OCF2A); }

	async command void CompareA.reset() { TIFR2 = 1 << OCF2A; }

// ----- COMPAREA: timer interrupt mask register (TIMSK), output compare interrupt enable (OCIE)

	async command void CompareA.start()
	{
		SET_BIT(TIMSK2, OCIE2A);

		call McuPowerState.update();
	}

	async command void CompareA.stop()
	{
		CLR_BIT(TIMSK2, OCIE2A);

		call McuPowerState.update();
	}

	async command bool CompareA.isOn() { return TIMSK2 & (1 << OCIE2A); }

// ----- COMPAREA: timer control register (TCCR), compare output mode (COM)

	async command void CompareA.setMode(uint8_t mode)
	{
		mode = (mode & 0x03) << 6;

		atomic
		{
			while( ASSR & (1 << TCR2AUB) )
				;

			TCCR2A = (TCCR2A & 0x3F) | mode;
		}

		call McuPowerState.update();
	}

	async command uint8_t CompareA.getMode()
	{
		return (TCCR2A >> 6) & 0x03;
	}

// ----- COMPAREA: timer control register (TCCR), force output compare (FOC)

	async command void CompareA.force()
	{
		atomic
		{
			while( ASSR & (1 << TCR2BUB) )
				;

			SET_BIT(TCCR2B, FOC2A);
		}

		call McuPowerState.update();
	}

// ----- MCUPOWER

	async command mcu_power_t McuPowerOverride.lowestState()
	{
		// wait for all changes to propagate
		while( ASSR & (1 << TCN2UB | 1 << OCR2AUB | 1 << OCR2BUB | 1 << TCR2AUB | 1 << TCR2BUB) )
			;

		// if we need to wake up by this clock
		if( TIMSK2 & (1 << TOIE2 | 1 << OCIE2A | 1 << OCIE2B) )
			return ATM128_POWER_SAVE;
		else
			return ATM128_POWER_DOWN;
	}
}
