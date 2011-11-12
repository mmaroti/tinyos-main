/*
* Copyright (c) 2011, University of Szeged
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
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/

#include <Tasklet.h>
#include <Si443xRadio.h>

module RadioStackP {
	provides {
		interface Si443xDriverConfig as Config;
		interface PacketTimeStamp<TRadio, uint32_t> as PacketTimeStampRadio;

		interface PacketFlag as TransmitPowerFlag;
		interface PacketFlag as RSSIFlag;
		interface PacketFlag as TimeSyncFlag;
	}
}

implementation {
	async command uint8_t Config.headerLength(message_t* msg)
	{
		return offsetof(message_t, data) - sizeof(si443xpacket_header_t);
	}

	async command uint8_t Config.maxPayloadLength()
	{
		return sizeof(si443xpacket_header_t) + TOSH_DATA_LENGTH;
	}

	async command uint8_t Config.metadataLength(message_t* msg) { return 0;	}
	async command uint8_t Config.headerPreloadLength()	{ return 7; }
	async command bool Config.requiresRssiCca(message_t* msg) { return FALSE; }
	
	async command bool PacketTimeStampRadio.isValid(message_t* msg)	{ return FALSE;	}
	async command uint32_t PacketTimeStampRadio.timestamp(message_t* msg) { return FALSE; }
	async command void PacketTimeStampRadio.clear(message_t* msg) { }
	async command void PacketTimeStampRadio.set(message_t* msg, uint32_t value)	{ }
	
	async command bool TransmitPowerFlag.get(message_t* msg) { return FALSE;	}
	async command void TransmitPowerFlag.set(message_t* msg)	{	}
	async command void TransmitPowerFlag.clear(message_t* msg)	{	}
	async command void TransmitPowerFlag.setValue(message_t* msg, bool value)	{	}
	
	async command bool RSSIFlag.get(message_t* msg) { return FALSE;	}
	async command void RSSIFlag.set(message_t* msg)	{	}
	async command void RSSIFlag.clear(message_t* msg)	{	}
	async command void RSSIFlag.setValue(message_t* msg, bool value)	{	}
	
	async command bool TimeSyncFlag.get(message_t* msg) { return FALSE;	}
	async command void TimeSyncFlag.set(message_t* msg)	{	}
	async command void TimeSyncFlag.clear(message_t* msg)	{	}
	async command void TimeSyncFlag.setValue(message_t* msg, bool value)	{	}
}
