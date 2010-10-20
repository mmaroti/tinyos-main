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

generic module AtmRtcTimerP(typedef precision, uint8_t mode)
{
	provides
	{
		interface Init;
		interface Counter<precision, uint16_t>;
		interface Alarm<precision, uint16_t>;
	}

	uses
	{
		interface HplAtmCounter<uint8_t>;
		interface HplAtmCompare<uint8_t> as HplAtmCompareA;

		interface Leds;
	}
}

implementation
{
	command error_t Init.init()
	{
		call Leds.led2On();

		call HplAtmCompareA.stop();
		call HplAtmCompareA.setMode(0);

		call HplAtmCounter.setMode(mode);
		call HplAtmCounter.start();

		return SUCCESS;
	}

	volatile uint8_t high;

	/*
	 * Without prescaler the interrupt occurs when the Timer goes from 0 to 1,
	 * so we can have two posible sequences of events
	 *
	 *	TST=0, CNT=0, TST=1, CNT=1, TST=1 ... TST=1, CNT=1, TST=0
	 *	TST=0, CNT=0, TST=0, CNT=1, TST=1 ... TST=1, CNT=1, TST=0
	 *
	 * With the prescaler enabled the interrupt occurs while the Timer is 0
	 * (one 32768 HZ tick after the Timer became 0), so we have one possibility:
	 *
	 *	TST=0, CNT=0, TST=1, CNT=0, TST=1 ... TST=1, CNT=0, TST=0
	 */

	async command uint16_t Counter.get()
	{
		uint8_t a, b;
		bool c;

		atomic
		{
			b = call HplAtmCounter.get();
			c = call HplAtmCounter.test();
			a = high;
		}

		if( c && b != 0 )
			a += 1;

		// overflow occurs when switching from 0 to 1.
		b -= 1;

		return (((uint16_t)a) << 8) + b;
	}

	async command bool Counter.isOverflowPending()
	{
		atomic return high == 0xFF && call HplAtmCounter.test();
	}

	async command void Counter.clearOverflow()
	{
		call HplAtmCounter.reset();
	}

	uint16_t alarm;
	uint8_t alarmWait;

	// called in atomic context
	async event void HplAtmCounter.overflow()
	{
		++high;

		if( high == 0 )
		{
//			call Leds.led2Toggle();
			signal Counter.overflow();
		}
	}

	default async event void Alarm.fired() { }

	// called in atomic context
	async event void HplAtmCompareA.fired()
	{
		uint8_t h = high;
		if( call HplAtmCounter.test() )
			h += 1;

		if( h == (alarm >> 8) )
		{
			call Leds.led1Toggle();

			call HplAtmCompareA.stop();
			signal Alarm.fired();
		}
	}

	async command void Alarm.stop()
	{
		call HplAtmCompareA.stop();
	}

	async command bool Alarm.isRunning()
	{
		return call HplAtmCompareA.isOn();
	}

	// callers make sure that time is always in the future
	void setAlarm(uint16_t time)
	{
		call HplAtmCompareA.set((uint8_t)time);

		alarm = time;

//		if( high == (time >> 8) )
//		{
			call HplAtmCompareA.reset();
			call HplAtmCompareA.start();
//		}
//		else
//			call HplAtmCompareA.stop();
	}

	async command void Alarm.startAt(uint16_t nt0, uint16_t ndt)
	{
		atomic
		{
			// current time + time needed to set alarm
			uint16_t n = call Counter.get() + 4;

			// if alarm is set in the future, where n-nt0 is the time passed since nt0
			if( (uint16_t)(n - nt0) < ndt )
				n = nt0 + ndt;

			setAlarm(n);
		}
	}

	async command void Alarm.start(uint16_t ndt)
	{
		atomic
		{
			uint16_t n = call Counter.get();

			// calculate the next alarm
			n += (4 > ndt) ? 4 : ndt;

			setAlarm(n);
		}
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
