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
* Author: Ali Baharev
*/

module LedHandlerP {
	
	provides interface LedHandler;
	
	uses interface Leds;

}

implementation{

#define DISABLED 0
#if DISABLED
	command void LedHandler.radioOn() {	}
	command void LedHandler.radioOff() { }
	command void LedHandler.diskReady() { }
	command void LedHandler.error() { }
	command void LedHandler.msgReceived() { }
	command void LedHandler.sampling() { }
	command void LedHandler.errorToggle() { }
	command void LedHandler.sendingToggle() { }
	command void LedHandler.led1On() { }
	command void LedHandler.led1Off() { }
	command void LedHandler.led2On() { }
	command void LedHandler.led2Off() { }
	command void LedHandler.led12Off() {	}
	command void LedHandler.set(uint8_t val) {	call Leds.set(val);	}
#else
	command void LedHandler.radioOn() {
		call Leds.led1On();
	}	

	command void LedHandler.radioOff() {
		call Leds.led1Off();
	}

	command void LedHandler.diskReady() {
		call Leds.led2Toggle();
	}

	command void LedHandler.error() {
		call Leds.led0On();
	}

	command void LedHandler.msgReceived(){
		call Leds.led2Toggle();
	}

	command void LedHandler.sampling(){
		call Leds.led2Toggle();
	}
	
	command void LedHandler.errorToggle() {
	    call Leds.led0Toggle();
	}
	
	command void LedHandler.sendingToggle(){
		call Leds.led2Toggle();		
	}
	
	command void LedHandler.led1On(){
		call Leds.led1On();
	}
	
	command void LedHandler.led1Off(){
		call Leds.led1Off();
	}
	
	command void LedHandler.led2On(){
		call Leds.led2On();
	}
	
	command void LedHandler.led2Off(){
		call Leds.led2Off();
	}
	
	command void LedHandler.led12Off(){
		call Leds.set(6);
	}
	
	command void LedHandler.set(uint8_t val) {
		
	}
#endif

}