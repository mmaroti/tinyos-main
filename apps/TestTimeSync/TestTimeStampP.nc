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

enum
{
	SEND_PERIOD = 100,	// in TMilli units
	BLINK_DELAY = 30000,	// in TMicro units
};

/*
	node 0 sends full packets at every SEND_PERIOD microseconds, node 1 receives them

	node 0 blinks LED 0 after BLINK_DELAY microseconds after the time stamp of the sent message
	node 1 blinks LED 1 after BLINK_DELAY microseconds after the time stamp of the received message
*/

module TestTimeStampP
{
	uses
	{
		interface Boot;
		interface SplitControl;
		interface AMSend;
		interface Receive;
		interface Leds;
		interface Timer<TMilli> as Timer;

		interface PacketTimeStamp<TMicro, uint32_t>;
		interface Alarm<TMicro, uint16_t>;
	}
}

implementation
{
	message_t msgBuffer;

	event void Boot.booted()
	{
		call SplitControl.start();
	}

	event void SplitControl.startDone(error_t err)
	{
		if( TOS_NODE_ID == 0 )
			call Timer.startPeriodic(SEND_PERIOD);
	}
	
	event void SplitControl.stopDone(error_t err) { }

	event void Timer.fired()
	{
		call AMSend.send(AM_BROADCAST_ADDR, &msgBuffer, TOSH_DATA_LENGTH);
	}

	void startAlarm(message_t* msg)
	{
		uint16_t time;

		if( call PacketTimeStamp.isValid(msg) )
		{
			time = BLINK_DELAY + (uint16_t) call PacketTimeStamp.timestamp(msg);

			atomic
			{
				call Alarm.start( time - call Alarm.getNow() );
			}
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t error)
	{
		startAlarm(msg);
  	}

	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t length)
	{
		startAlarm(msg);
		return msg;
	}

	async event void Alarm.fired()
	{
		if( TOS_NODE_ID == 0 )
			call Leds.led0Toggle();
		else
			call Leds.led1Toggle();
	}
}
