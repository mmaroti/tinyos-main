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

generic module AtmAsyncCounterP(typedef precision, uint8_t prescaler, uint8_t mode)
{
	provides
	{
		interface Init;
		interface Counter<precision, uint16_t>;
		interface Alarm<precision, uint16_t>;
	}

	uses
	{
		interface HplAtmTimer<uint8_t> as Timer;
		interface HplAtmCompare<uint8_t> as Compare;

		interface Leds;
	}
}

implementation
{
	command error_t Init.init()
	{
		call Compare.stop();
		call Compare.setMode(0);

		call Timer.setMode(mode);
		call Timer.setScale(prescaler);
		call Timer.start();

		return SUCCESS;
	}

	volatile uint8_t high;

	async command uint16_t Counter.get()
	{
		uint8_t a, b;

		atomic
		{
			b = call Timer.get();
			a = high;
			if( call Timer.test() )
			{
				a += 1;
				b = call Timer.get();
			}
		}

		// overflow occurs when the counter goes from 0 to 1
		b -= 1;

		return (((uint16_t)a) << 8) + b;
	}

	async command bool Counter.isOverflowPending()
	{
		atomic return high == 0xFF && call Timer.test();
	}

	async command void Counter.clearOverflow()
	{
		call Timer.reset();
	}

	uint16_t alarm;
	bool started;

	// called in atomic context
	async event void Timer.overflow()
	{
		++high;

		if( high == 0 )
			signal Counter.overflow();

		if( high == (alarm >> 8) && started )
		{
			if( ((uint8_t)alarm) > 3 )
				call Compare.reset();

			call Compare.start();
		}
	}

	async event void Compare.fired()
	{
		started = FALSE;
		signal Alarm.fired();
	}

	async command void Alarm.stop()
	{
		call Compare.stop();
		atomic started = FALSE;
	}

	async command bool Alarm.isRunning()
	{
		atomic return started;
	}

	async command void Alarm.startAt(uint16_t nt0, uint16_t ndt)
	{
		uint16_t n, a;

		// current time + time needed to set alarm
		n = call Counter.get() + 3;

		// the amout of time passed since nt0
		a = n - nt0;

		// if alarm is set in the past or too close to now
		if( a >= ndt )
			a = n;
		else 
			a = nt0 + ndt;

		atomic
		{
			alarm = a;
			started = TRUE;

			call Compare.set((uint8_t)alarm);
			call Compare.reset();
			if( high == (alarm >> 8) )
				call Compare.start();
		}
	}

	async command void Alarm.start(uint16_t ndt)
	{
		call Alarm.startAt(call Counter.get(), ndt);
	}

	async command uint16_t Alarm.getNow()
	{
		return call Counter.get();
	}

	async command uint16_t Alarm.getAlarm()
	{
		return alarm;
	}
}
