/*
 * Copyright (c) 2009, Vanderbilt University
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
 * Author: Miklos Maroti, Gabor Pap, Janos Sallai
 */

//#include "GradientField.h"



generic module GradientFieldP(typedef payload_t)
{
	provides
	{
		interface DfrfSend as Send;
		interface DfrfReceive as Receive;
		interface GradientField;
	}
	uses
	{
		interface AMPacket;
		interface DfrfSend;
		interface DfrfReceive;
		interface Leds;
	}
}

implementation
{
	typedef nx_struct gradientfield_packet {
		nx_uint16_t rootAddress;	// address of the root of the gradient field
		payload_t payload;
		nx_uint8_t hopCount;	// hop count of the sender
	} gradient_field_packet_t;
	
	
	uint16_t rootAddress = 0xffff;
	uint16_t effectiveRootAddress = 0xffff;
	uint16_t hopCountSum;
	uint16_t effectiveHopCountSum;
	uint8_t  msgCount;
	uint8_t  effectiveMsgCount;


	/**** hop count ****/
	
	command error_t Send.send(void *send){
		error_t err;
		gradient_field_packet_t data;
		
		rootAddress = effectiveRootAddress = call AMPacket.address();

		hopCountSum = effectiveHopCountSum = 0;
		msgCount = effectiveMsgCount = 0;		
		
		
		data.rootAddress = rootAddress;
		data.hopCount = 0;
		data.payload=*((payload_t*)send);
		

		err=call DfrfSend.send(&data);
		call Leds.set(7);
		return err;
	}
	
	command void GradientField.beacon(){
		payload_t dummy;
		call Send.send(&dummy);
	}

	command am_addr_t GradientField.rootAddress()
	{
		return effectiveRootAddress;
	}

	command void GradientField.setRootAddress(am_addr_t ra)
	{
		rootAddress = effectiveRootAddress = ra;
	}

	command uint16_t GradientField.hopCount()
	{
		if( effectiveMsgCount == 0 )
			return 0xffff;
		else
			return (effectiveHopCountSum << 2) / effectiveMsgCount;
	}

	command void GradientField.setHopCount(uint16_t hc)
	{
		if(hc == 0xffff) {
			hopCountSum = effectiveHopCountSum = 0;
			msgCount = effectiveMsgCount = 0;
		} else {
			hopCountSum = effectiveHopCountSum = hc;
			msgCount = effectiveMsgCount = 4;
		}
	}

	/**** implementation ****/

	event bool DfrfReceive.receive(void* raw_data)
	{
		gradient_field_packet_t *data=(gradient_field_packet_t*)raw_data;

		// detect a new beaconing round
		if(( rootAddress ) != ( data->rootAddress  )) {
			rootAddress = data->rootAddress;
			msgCount = 0;
		}


		hopCountSum += ++(data->hopCount);
		++msgCount;

		// update last* values if enough messages have been received in the current beaconing round
		if(msgCount > effectiveMsgCount / 2) {
			effectiveMsgCount = msgCount;
			effectiveRootAddress = rootAddress;
			effectiveHopCountSum = hopCountSum;
		}
		
		signal Receive.receive(&(data->payload));

		return TRUE;
	}

}
