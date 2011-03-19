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
	REPORT_DELAY_0 = 10,
	REPORT_DELAY_1 = 20,
	REPORT_DELAY_2 = 30,
};

/*
 * NODE 0 is sending reference messages with payload length 1 to NODES 1 
 * and 2, and blinks LED 0. All nodes make a reference time stmap for the 
 * reference messages. When NODE 1 receives a reference message, it sends 
 * a test message with length 100 to NODES 0 and 2 and blinks LED 1. All
 * NODES records the time stmap of the test message too. Then they report
 * the difference of the two time stamps via DiagMsgs to a basestation.
 */

module TestTimeStampP
{
	uses
	{
		interface Boot;
		interface SplitControl;
		interface AMSend as RefSend;
		interface Receive as RefReceive;
		interface AMSend as TestSend;
		interface Receive as TestReceive;
		interface Leds;
		interface Timer<TMilli> as SendTimer;
		interface Timer<TMilli> as ReportTimer;
		interface PacketTimeStamp<TMicro, uint32_t>;
		interface DiagMsg;
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
			call SendTimer.startPeriodic(SEND_PERIOD);
	}

	event void SplitControl.stopDone(error_t err) { }

	uint32_t refTime;
	uint32_t testTime;

	event void SendTimer.fired()
	{
		if( TOS_NODE_ID == 0 )
		{
			call Leds.led0Toggle();
			call RefSend.send(AM_BROADCAST_ADDR, &msgBuffer, 1);
		}
	}

	event void ReportTimer.fired()
	{
		if( call DiagMsg.record() )
		{
			call DiagMsg.uint8(TOS_NODE_ID);
			call DiagMsg.uint32(testTime - refTime);
			call DiagMsg.uint32(refTime);
			call DiagMsg.send();
		}
	}

	event void RefSend.sendDone(message_t* msg, error_t error)
	{
		if( TOS_NODE_ID == 0 )
		{
			if( call PacketTimeStamp.isValid(msg) )
				refTime = call PacketTimeStamp.timestamp(msg);
			else
				refTime = 0;
		}
	}

	event message_t* RefReceive.receive(message_t* msg, void* payload, uint8_t length)
	{
		if( call PacketTimeStamp.isValid(msg) )
			refTime = call PacketTimeStamp.timestamp(msg);
		else
			refTime = 0;

		if( TOS_NODE_ID == 1 )
		{
			call Leds.led1Toggle();
			call TestSend.send(AM_BROADCAST_ADDR, &msgBuffer, TOSH_DATA_LENGTH);
		}

		return msg;
	}

	event void TestSend.sendDone(message_t* msg, error_t error)
	{
		if( TOS_NODE_ID == 1 )
		{
			if( call PacketTimeStamp.isValid(msg) )
				testTime = call PacketTimeStamp.timestamp(msg);
			else
				testTime = 0;

			call ReportTimer.startOneShot(REPORT_DELAY_1);
		}
  	}

	event message_t* TestReceive.receive(message_t* msg, void* payload, uint8_t length)
	{
		if( TOS_NODE_ID != 1 )
		{
			if( call PacketTimeStamp.isValid(msg) )
				testTime = call PacketTimeStamp.timestamp(msg);
			else
				testTime = 0;
		}

		if( TOS_NODE_ID == 2 )
		{
			call Leds.led2Toggle();
			call ReportTimer.startOneShot(REPORT_DELAY_2);
		}
		else if( TOS_NODE_ID == 0 )
			call ReportTimer.startOneShot(REPORT_DELAY_0);

		return msg;
	}
}
