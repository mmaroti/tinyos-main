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
#ifdef __RF230RADIO_H__
	#include <RadioConfig.h>
#elif defined(__CC2420_H__)
	#include <Timer.h>
#endif
configuration ActiveMessageC
{
	provides
	{
//interfaces for both chips
		interface SplitControl;

		interface AMSend[am_id_t id];
		interface Receive[am_id_t id];
		interface Receive as Snoop[am_id_t id];
		
		interface Packet;
		interface AMPacket;

		interface PacketAcknowledgements;
		interface LowPowerListening;

		
		interface PacketTimeStamp<TMilli, uint32_t> as PacketTimeStampMilli;
//interfaces for only the rf230
#ifdef __RF230RADIO_H__
		interface SendNotifier[am_id_t id];
		interface PacketTimeStamp<TMicro, uint32_t> as PacketTimeStampMicro;
		interface RadioChannel;
		interface PacketLink;
#elif defined(__CC2420_H__)
		interface PacketTimeStamp<T32khz, uint32_t> as PacketTimeStamp32khz;
#endif
	}
}

implementation
{
#ifdef __RF230RADIO_H__
	components RF230ActiveMessageC as MessageC;
#elif defined(__CC2420_H__)
	components CC2420ActiveMessageC as MessageC;
#endif
	components ActiveMessageGridP;
	SplitControl = MessageC;
	AMSend = MessageC;
#ifdef AM_FILTER_GRID
	Receive = ActiveMessageGridP.Receive;
	Snoop = ActiveMessageGridP.Snoop;
	ActiveMessageGridP.AMPacket->MessageC;
	ActiveMessageGridP.SubSnoop->MessageC.Snoop;
	ActiveMessageGridP.SubReceive->MessageC.Receive;
#elif defined(AM_FILTER_RANDOM)

#else
	Receive = MessageC.Receive;
	Snoop = MessageC.Snoop;
#endif
	
	
	Packet = MessageC;
	AMPacket = MessageC;

	PacketAcknowledgements = MessageC;
	LowPowerListening = MessageC;
	
#ifdef __RF230RADIO_H__	
	SendNotifier = MessageC;
	PacketLink = MessageC;
	RadioChannel = MessageC;

	PacketTimeStampMilli = MessageC;
	PacketTimeStampMicro = MessageC;
#endif
#ifdef __CC2420_H__
	components CC2420PacketC;
	PacketTimeStamp32khz = CC2420PacketC;
	PacketTimeStampMilli = CC2420PacketC;
#endif
}
