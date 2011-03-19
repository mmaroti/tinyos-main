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

module AppTestCounterP
{
	uses
	{
		interface Boot;
		interface Leds;

		interface DiagMsg;
		interface SplitControl;

		interface Counter<T32khz, uint16_t>;
	}
}

implementation
{
	int16_t minStep;
	int16_t maxStep;

	task void testStep()
	{
		uint16_t a,b;
		int16_t d;

		atomic
		{
			a = call Counter.get();
			b = call Counter.get();
		}

//		if( (a & 0xFF) == 0 )
		{
			d = b-a;

			if( d < minStep )
				minStep = d;
			if( d > maxStep )
				maxStep = d;
		}

		post testStep();
	}

	task void report()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.str("step");
			call DiagMsg.int16(minStep);
			call DiagMsg.int16(maxStep);
			call DiagMsg.send();
		}

		minStep = 32767;
		maxStep = -32767;

	}

	async event void Counter.overflow()
	{
		call Leds.led2Toggle();
		post report();
	}

	event void Boot.booted()
	{
		call SplitControl.start();
		post testStep();
	}

	event void SplitControl.startDone(error_t result) { }
	event void SplitControl.stopDone(error_t result) { }
}
