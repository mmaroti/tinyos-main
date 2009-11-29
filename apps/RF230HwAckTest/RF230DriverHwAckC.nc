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

#include <RadioConfig.h>
#include <RF230DriverLayer.h>

configuration RF230DriverHwAckC
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

		interface PacketAcknowledgements;
	}

	uses
	{
		interface RF230DriverConfig as Config;
		interface PacketTimeStamp<TRadio, uint32_t>;
		interface Ieee154PacketLayer;
	}
}

implementation
{
	components RF230DriverHwAckP, HplRF230C, BusyWaitMicroC, TaskletC, MainC, RadioAlarmC;

	RadioState = RF230DriverHwAckP;
	RadioSend = RF230DriverHwAckP;
	RadioReceive = RF230DriverHwAckP;
	RadioCCA = RF230DriverHwAckP;
	RadioPacket = RF230DriverHwAckP;

	LocalTimeRadio = HplRF230C;

	Config = RF230DriverHwAckP;

	PacketTransmitPower = RF230DriverHwAckP.PacketTransmitPower;
	components new MetadataFlagC() as TransmitPowerFlagC;
	RF230DriverHwAckP.TransmitPowerFlag -> TransmitPowerFlagC;

	PacketRSSI = RF230DriverHwAckP.PacketRSSI;
	components new MetadataFlagC() as RSSIFlagC;
	RF230DriverHwAckP.RSSIFlag -> RSSIFlagC;

	PacketTimeSyncOffset = RF230DriverHwAckP.PacketTimeSyncOffset;
	components new MetadataFlagC() as TimeSyncFlagC;
	RF230DriverHwAckP.TimeSyncFlag -> TimeSyncFlagC;

	PacketLinkQuality = RF230DriverHwAckP.PacketLinkQuality;
	PacketTimeStamp = RF230DriverHwAckP.PacketTimeStamp;

	RF230DriverHwAckP.LocalTime -> HplRF230C;

	RF230DriverHwAckP.RadioAlarm -> RadioAlarmC.RadioAlarm[unique("RadioAlarm")];
	RadioAlarmC.Alarm -> HplRF230C.Alarm;

	RF230DriverHwAckP.SELN -> HplRF230C.SELN;
	RF230DriverHwAckP.SpiResource -> HplRF230C.SpiResource;
	RF230DriverHwAckP.FastSpiByte -> HplRF230C;

	RF230DriverHwAckP.SLP_TR -> HplRF230C.SLP_TR;
	RF230DriverHwAckP.RSTN -> HplRF230C.RSTN;

	RF230DriverHwAckP.IRQ -> HplRF230C.IRQ;
	RF230DriverHwAckP.Tasklet -> TaskletC;
	RF230DriverHwAckP.BusyWait -> BusyWaitMicroC;

#ifdef RADIO_DEBUG
	components DiagMsgC;
	RF230DriverHwAckP.DiagMsg -> DiagMsgC;
#endif

	MainC.SoftwareInit -> RF230DriverHwAckP.SoftwareInit;

	components RealMainP;
	RealMainP.PlatformInit -> RF230DriverHwAckP.PlatformInit;

	components new MetadataFlagC(), ActiveMessageAddressC;
	RF230DriverHwAckP.AckReceivedFlag -> MetadataFlagC;
	RF230DriverHwAckP.ActiveMessageAddress -> ActiveMessageAddressC;
	PacketAcknowledgements = RF230DriverHwAckP;
	Ieee154PacketLayer = RF230DriverHwAckP;
}
