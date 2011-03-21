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

#include "TimerConfig.h"

configuration AppGetTracesC
{ 
} 

implementation
{
	components MainC, AppGetTracesP, LedsC, DiagMsgC, SerialActiveMessageC;

	AppGetTracesP.Boot -> MainC;
	AppGetTracesP.Leds -> LedsC;
	AppGetTracesP.SplitControl -> SerialActiveMessageC;
	AppGetTracesP.DiagMsg -> DiagMsgC;

	components HplAtmRfa1Timer2AsyncC;
	components new AtmegaAsyncTimerP(TRtc, RTC_TIMER_MODE, 4);
	AtmegaAsyncTimerP.AtmegaCounter -> HplAtmRfa1Timer2AsyncC;
	AtmegaAsyncTimerP.AtmegaCompare -> HplAtmRfa1Timer2AsyncC.Compare[0];
	AppGetTracesP.Counter -> AtmegaAsyncTimerP;
	AppGetTracesP.Alarm -> AtmegaAsyncTimerP;
	components McuInitC;
	McuInitC.TimerInit -> AtmegaAsyncTimerP.Init;

/*
	components HplAtmRfa1Timer2AsyncC;
	AppGetTracesP.Counter -> HplAtmRfa1Timer2AsyncC;
	AppGetTracesP.Compare -> HplAtmRfa1Timer2AsyncC.Compare[0];
*/
/*
	components HplAtmRfa1Timer1C;
	AppGetTracesP.Counter -> HplAtmRfa1Timer1C;
*/
}
