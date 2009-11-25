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
* Author: Zoltan Kincses
*/

#include "MicReadAdc.h"


configuration MicReadAdcMultipleAppC
{
}
implementation {
  
	components new AMSenderC(AM_DATA_MSG) as Send;
	components new AMReceiverC(AM_CTRL_MSG) as Receive;
	components new AlarmMicro32C() as Alarm;
	components Atm128AdcC;
	components MicReadAdcMultipleC;
	components ActiveMessageC;
	components MainC;
	components LedsC;
	components MicaBusC;
  
	MicReadAdcMultipleC.Leds -> LedsC;
	MicReadAdcMultipleC.Boot -> MainC;
	MicReadAdcMultipleC.Alarm -> Alarm;
	MicReadAdcMultipleC.Uart -> ActiveMessageC;
	MicReadAdcMultipleC.Microphone -> MicrophoneC;
	MicReadAdcMultipleC.AMSend -> Send;
	MicReadAdcMultipleC.Receive -> Receive;
	MicReadAdcMultipleC.AdcResource -> Atm128AdcC.Resource[unique(UQ_ATM128ADC_RESOURCE)];
	MicReadAdcMultipleC.Atm128AdcMultiple -> Atm128AdcC;
	MicReadAdcMultipleC.MicAdcChannel -> MicaBusC.Adc2;

	components new TimerMilliC() as MTimer;
	components new Atm128I2CMasterC() as I2CPot;
	components MicrophoneC;
 
	MicrophoneC.Timer -> MTimer;
	MicrophoneC.MicPower  -> MicaBusC.PW3;
	MicrophoneC.MicMuxSel -> MicaBusC.PW6;
	MicrophoneC.I2CResource -> I2CPot;
	MicrophoneC.I2CPacket -> I2CPot;
}
