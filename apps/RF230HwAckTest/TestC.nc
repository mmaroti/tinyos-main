/*
 * Copyright (c) 2007, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Miklos Maroti
 */

configuration TestC
{
}

implementation
{
	components MainC, TestP, DiagMsgC, LedsC;
	components ActiveMessageC, SerialActiveMessageC;
	components new TimerMilliC() as SendTimerC;

	TestP.Boot -> MainC;
	TestP.DiagMsg -> DiagMsgC;
	TestP.SendTimer -> SendTimerC;

	TestP.SerialControl -> SerialActiveMessageC;
	TestP.RadioControl -> ActiveMessageC;

	TestP.PacketAcknowledgements -> ActiveMessageC;
	TestP.AMPacket -> ActiveMessageC;
	TestP.Packet -> ActiveMessageC;
	TestP.AMSend -> ActiveMessageC.AMSend[0x17];
	TestP.Receive -> ActiveMessageC.Receive[0x17];

	TestP.Leds -> LedsC;
}
