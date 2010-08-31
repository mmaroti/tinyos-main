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

module Idg300P
{
	provides 
	{
		interface StdControl;
		interface Init;
	}
}

implementation
{
	command error_t Init.init()
	{
		// set up ADC channels
		TOSH_MAKE_ADC_1_INPUT();   // x
		TOSH_MAKE_ADC_2_INPUT();   // z
		TOSH_MAKE_ADC_6_INPUT();   // y

		TOSH_SEL_ADC_1_MODFUNC();
		TOSH_SEL_ADC_2_MODFUNC();
		TOSH_SEL_ADC_6_MODFUNC();

		return SUCCESS;
	}

	command error_t StdControl.start()
	{
		// gyro enable low
		TOSH_CLR_PROG_OUT_PIN();

		return SUCCESS;
	}

	command error_t StdControl.stop()
	{
		// disable gyro
		TOSH_SET_PROG_OUT_PIN();

		return SUCCESS;
	}
}
