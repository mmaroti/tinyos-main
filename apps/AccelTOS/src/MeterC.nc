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

configuration MeterC
{ 
	provides interface Meter;
	provides interface StdControl;

} 

implementation
{ 
	components MeterP, /*ActiveMessageC,*/ new TimerMilliC();
	components LedHandlerC , RadioDiagMsgC; // FIXME It was unused anyhow...
	components SimpleFileC; // FIXME There should be only one SimpleFileC
	MeterP.DiagMsg -> RadioDiagMsgC;
	
	Meter = MeterP;
	StdControl = MeterP;
	MeterP.Timer -> TimerMilliC;
	MeterP.LedHandler -> LedHandlerC;

	components ShimmerAdcC;
	MeterP.ShimmerAdc -> ShimmerAdcC;

	components Mma7260P;
	MeterP.AccelInit -> Mma7260P;
	MeterP.Accel -> Mma7260P;

// FIXME Only one component should turn on the disc
// FIXME Turn off the disc? (Data corruption)
	components BufferedFlashP; // FIXME Move these to a new configuration!!!
	MeterP.BufferedFlash -> BufferedFlashP;
	BufferedFlashP.SimpleFile -> SimpleFileC;
}
