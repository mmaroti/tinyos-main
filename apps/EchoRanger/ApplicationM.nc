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

#include "EchoRanger.h"

module ApplicationM
{
	uses
	{
		interface Boot;
		interface Leds;
		interface Timer<TMilli> as TimerMilli;
		interface SplitControl as AMControl;
		interface Read<echorange_t*> as EchoRanger;
	}
}

implementation
{
	enum
	{
		ECHO_TIMER = 5 * 1024,	// every 5 seconds
	};

	event void Boot.booted()
	{
		call AMControl.start();
	}

	event void AMControl.startDone(error_t err)
	{
		if( err == SUCCESS )
			call TimerMilli.startPeriodic(ECHO_TIMER);
		else
			call AMControl.start();
	}

	event void AMControl.stopDone(error_t err)
	{
	}

	event void TimerMilli.fired()
	{
		call Leds.led0Toggle();
		call EchoRanger.read();
	}
	
 	event void EchoRanger.readDone(error_t result, echorange_t* range)
	{
	}
}
