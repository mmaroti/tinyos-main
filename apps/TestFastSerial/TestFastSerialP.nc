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

module TestFastSerialP
{
	uses
	{
		interface Boot;
		interface SplitControl as SerialControl;
		interface AMSend as SerialSend;
		interface Leds;

#ifdef TEST_WITH_RADIO
		interface SplitControl as RadioControl;
		interface AMSend as RadioSend;
#endif
	}
}

implementation
{
	message_t serialMsg;
	message_t radioMsg;

	event void Boot.booted()
	{
		uint8_t i;

		for(i = 0; i < TOSH_DATA_LENGTH; ++i)
			serialMsg.data[i] = i;

		call SerialControl.start();
#ifdef TEST_WITH_RADIO
		call RadioControl.start();
#endif
	}

	event void SerialControl.startDone(error_t err)
	{
		call SerialSend.send(AM_BROADCAST_ADDR, &serialMsg, TOSH_DATA_LENGTH);
	}
	
	event void SerialControl.stopDone(error_t err) { }

	event void SerialSend.sendDone(message_t* bufPtr, error_t error)
	{
		call Leds.led2Toggle();
		serialMsg.data[0] += 1;
	   	call SerialSend.send(AM_BROADCAST_ADDR, &serialMsg, TOSH_DATA_LENGTH);
  	}

#ifdef TEST_WITH_RADIO
	event void RadioControl.startDone(error_t err)
	{
		call RadioSend.send(AM_BROADCAST_ADDR, &radioMsg, TOSH_DATA_LENGTH);
	}
	
	event void RadioControl.stopDone(error_t err) { }

	event void RadioSend.sendDone(message_t* bufPtr, error_t error)
	{
		call Leds.led1Toggle();
	   	call RadioSend.send(AM_BROADCAST_ADDR, &radioMsg, TOSH_DATA_LENGTH);
  	}
#endif
}
