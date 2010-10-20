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

generic module AtmMcuAlarmP(typedef precision, uint8_t mode, uint16_t mindt)
{
	provides
	{
		interface Init;
		interface Alarm<precision, uint16_t>;
	}

	uses
	{
		interface HplAtmCounter<uint16_t>;
		interface HplAtmCompare<uint16_t>;
	}
}

implementation
{
	command error_t Init.init()
	{
		call HplAtmCompare.stop();
		call HplAtmCompare.setMode(mode);

		return SUCCESS;
	}

	default async event void Alarm.fired() { }

	// called in atomic context
	async event void HplAtmCompare.fired()
	{
		call HplAtmCompare.stop();
		signal Alarm.fired();
	}

	async command void Alarm.stop()
	{
		call HplAtmCompare.stop();
	}

	async command bool Alarm.isRunning()
	{
		return call HplAtmCompare.isOn();
	}

	// callers make sure that time is always in the future
	void setAlarm(uint16_t time)
	{
		call HplAtmCompare.set(time);
		call HplAtmCompare.reset();
		call HplAtmCompare.start();
	}

	async command void Alarm.startAt(uint16_t nt0, uint16_t ndt)
	{
		atomic
		{
			// current time + time needed to set alarm
			uint16_t n = call HplAtmCounter.get() + mindt;

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
			uint16_t n = call HplAtmCounter.get();

			// calculate the next alarm
			n += (mindt > ndt) ? mindt : ndt;

			setAlarm(n);
		}
	}

	async command uint16_t Alarm.getNow()
	{
		return call HplAtmCounter.get();
	}

	async command uint16_t Alarm.getAlarm()
	{
		return call HplAtmCompare.get();
	}

	async event void HplAtmCounter.overflow() { }
}
