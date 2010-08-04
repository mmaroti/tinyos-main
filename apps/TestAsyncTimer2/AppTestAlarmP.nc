/** Copyright (c) 2009, University of Szeged
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

module AppTestAlarmP
{
	uses
	{
		interface Boot;
		interface Leds;

		interface DiagMsg;
		interface SplitControl;

		interface Counter<T32khz, uint16_t>;
		interface Alarm<T32khz, uint16_t>;
	}
}

implementation
{
	bool reporting = FALSE;

	uint16_t alarmCount;
	uint16_t targetAlarm;
	int16_t minError;
	int16_t maxError;

	task void testAlarm();

	async event void Alarm.fired()
	{
		int16_t error = call Counter.get() - targetAlarm;

		if( error < minError )
			minError = error;
		if( error > maxError )
			maxError = error;

		++alarmCount;

		call Leds.led1Toggle();
		post testAlarm();
	}

	enum
	{
		ALARM_RANDOM = 1,
		ALARM_RANDOM_OFFSET = 4000,
		ALARM_BASE = 0x30,
		ALARM_TARGET = 0x70,
	};

	task void testAlarm()
	{
		uint16_t a;

		if( reporting )
			return;

		if( ALARM_RANDOM )
		{
			atomic
			{
				a = call Counter.get();
				call Alarm.startAt(a, ALARM_RANDOM_OFFSET);
				targetAlarm = a + ALARM_RANDOM_OFFSET;
			}
		}
		else
		{
			atomic
			{
				a = call Counter.get();
				if( (a & 0xFF) == ALARM_BASE )
				{
					call Alarm.startAt(a, ALARM_TARGET-ALARM_BASE);
					targetAlarm = a + (uint16_t)(ALARM_TARGET-ALARM_BASE);
				}
				else
					post testAlarm();
			}
		}
	}

	task void report()
	{
		int16_t a, b;
		uint16_t c;

		atomic
		{
			a = minError;
			b = maxError;
			c = alarmCount;
			minError = 32767;
			maxError = -32767;
			alarmCount = 0;
		}

		if( call DiagMsg.record() )
		{
			call DiagMsg.str("error");
			call DiagMsg.int16(a);
			call DiagMsg.int16(b);
			call DiagMsg.uint16(c);
			call DiagMsg.send();
		}

	}

	task void toggleReporting()
	{
		if( reporting )
		{
			call Leds.led2Off();
			reporting = FALSE;
			call SplitControl.stop();
		}
		else
		{
			call Leds.led2On();
			reporting = TRUE;
			call SplitControl.start();
		}
	}

	async event void Counter.overflow()
	{
		post toggleReporting();
	}

	task void busy()
	{
		post busy();
	}

	event void Boot.booted()
	{
//		post busy();
	}

	event void SplitControl.startDone(error_t result)
	{
		post report();
	}

	event void SplitControl.stopDone(error_t result)
	{
		post testAlarm();
	}
}
