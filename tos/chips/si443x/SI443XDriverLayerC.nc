/*
 * Copyright (c) 2007, Vanderbilt University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the copyright holder nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Miklos Maroti
 * Author: Krisztian Veress
 */

#include <RadioConfig.h>
#include <SI443XDriverLayer.h>

configuration SI443XDriverLayerC
{
	provides
	{
		interface RadioState;
		interface RadioSend;
		interface RadioReceive;
		interface RadioCCA;
		interface RadioPacket;

		interface PacketField<uint8_t> as PacketTransmitPower;
		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketTimeSyncOffset;
		interface PacketField<uint8_t> as PacketLinkQuality;

		interface LocalTime<TRadio> as LocalTimeRadio;
		interface Alarm<TRadio, tradio_size>;
	}

	uses
	{
		interface SI443XDriverConfig as Config;
		interface PacketTimeStamp<TRadio, uint32_t>;

		interface PacketFlag as TransmitPowerFlag;
		interface PacketFlag as RSSIFlag;
		interface PacketFlag as TimeSyncFlag;
		interface RadioAlarm;
	}
}

implementation
{
	components SI443XDriverLayerP, HplSi443xC, BusyWaitMicroC, TaskletC, MainC;

    // provides
	RadioState = SI443XDriverLayerP;
	RadioSend = SI443XDriverLayerP;
	RadioReceive = SI443XDriverLayerP;
	RadioCCA = SI443XDriverLayerP;
	RadioPacket = SI443XDriverLayerP;

    PacketTransmitPower = SI443XDriverLayerP.PacketTransmitPower;
	PacketRSSI = SI443XDriverLayerP.PacketRSSI;
	PacketTimeSyncOffset = SI443XDriverLayerP.PacketTimeSyncOffset;
	PacketLinkQuality = SI443XDriverLayerP.PacketLinkQuality;
			
	LocalTimeRadio = HplSi443xC;
    Alarm = HplSi443xC.Alarm;
	
	// uses
	Config = SI443XDriverLayerP;
	PacketTimeStamp = SI443XDriverLayerP.PacketTimeStamp;
	
	TransmitPowerFlag = SI443XDriverLayerP.TransmitPowerFlag;
	RSSIFlag = SI443XDriverLayerP.RSSIFlag;
	TimeSyncFlag = SI443XDriverLayerP.TimeSyncFlag;
	RadioAlarm = SI443XDriverLayerP.RadioAlarm;
	
    SI443XDriverLayerP.SDN -> HplSi443xC.SDN;
    SI443XDriverLayerP.NSEL -> HplSi443xC.NSEL;
//	SI443XDriverLayerP.GPIO0 -> HplSi443xC.IO0;
//	SI443XDriverLayerP.NIRQ -> HplSi443xC;
	
	SI443XDriverLayerP.FastSpiByte -> HplSi443xC;	
	SI443XDriverLayerP.SpiResource -> HplSi443xC.SpiResource;
	
	SI443XDriverLayerP.BusyWait -> BusyWaitMicroC;
    SI443XDriverLayerP.LocalTime -> HplSi443xC;
    
	SI443XDriverLayerP.Tasklet -> TaskletC;

#ifdef RADIO_DEBUG
	components DiagMsgC;
	SI443XDriverLayerP.DiagMsg -> DiagMsgC;
#endif

	MainC.SoftwareInit -> SI443XDriverLayerP.SoftwareInit;

	components RealMainP;
	RealMainP.PlatformInit -> SI443XDriverLayerP.PlatformInit;
}
