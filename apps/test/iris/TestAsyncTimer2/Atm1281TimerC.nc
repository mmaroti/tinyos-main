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

#include "HplAtm1281Timer.h"

configuration Atm1281TimerC
{
	provides
	{
		interface Counter<TMicro, uint16_t> as Counter1;

		interface Counter<T32khz, uint16_t> as Counter2;
		interface Alarm<T32khz, uint16_t> as Alarm2[uint8_t id];
	}
}

implementation
{
	components HplAtm1281TimerC, MainC;

	components new AtmAsyncTimerP(T32khz, ATM1281_CLK8_NORMAL | ATM1281_WGM8_NORMAL | ATM1281_ASYNC_ON);
	Counter2 = AtmAsyncTimerP;
	Alarm2[0] = AtmAsyncTimerP;
	AtmAsyncTimerP.Timer -> HplAtm1281TimerC.Timer2;
	AtmAsyncTimerP.CompareA -> HplAtm1281TimerC.Compare2[0];
	MainC.SoftwareInit -> AtmAsyncTimerP;

	components new AtmSynchTimerP(TMicro, ATM1281_CLK16_DIVIDE_256 | ATM1281_WGM16_NORMAL);
	Counter1 = AtmSynchTimerP;
	AtmSynchTimerP.Timer -> HplAtm1281TimerC.Timer1;
	MainC.SoftwareInit -> AtmSynchTimerP;
}
