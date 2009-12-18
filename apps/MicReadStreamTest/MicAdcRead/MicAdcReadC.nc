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

#include "MicAdcRead.h"

module MicAdcReadC
{
	uses {
		interface Alarm<TMicro, uint32_t>;
		interface Leds;
		interface Boot;
		interface Receive;
		interface AMSend;
		interface SplitControl as Radio;
		interface Resource as AdcResource;
		interface Atm128AdcMultiple;
		interface MicaBusAdc as MicAdcChannel;
		interface SplitControl as Microphone;
  	}
}
implementation {
  
  	norace uint32_t sampleNum;
	norace bool firstTime=TRUE;
	norace bool sampling=TRUE;
	norace bool adcStarted=FALSE,micStarted=FALSE;
	message_t msg;
	
	inline void dataSend()
	{
		data_msg_t* packet = (data_msg_t*)(call AMSend.getPayload(&msg, sizeof(data_msg_t)));
    	packet-> sampleNum = sampleNum;
		call AMSend.send(AM_BROADCAST_ADDR, &msg, sizeof(data_msg_t));
    }

   	event void Boot.booted()
	{
		call Radio.start();
		call AdcResource.request();
		call Microphone.start();
	}
  
	event void Radio.startDone(error_t err)
	{
		if(err!=SUCCESS)
		{
			call Radio.start();
		}
	}
	   	
   	event void AdcResource.granted()
  	{
		adcStarted=TRUE;
		call Leds.led0On();
	}
	
	event void Microphone.startDone(error_t error)
  	{
    	if (error != SUCCESS)
    	{
	   		call Microphone.start();
       	}
    	micStarted=TRUE;
	   	call Leds.led1On();
	}
   	
   	event message_t* Receive.receive(message_t* message ,void* payload, uint8_t len)
	{
		if (len == sizeof(ctrl_msg_t))
		{
			ctrl_msg_t *ctrl = (ctrl_msg_t*)payload;
			if (ctrl->instr == 's')
			{
				sampleNum=0;
				firstTime=TRUE;
				sampling = TRUE;
				if(adcStarted & micStarted)
				{
					switch (ctrl->preScale)
					{
						case 0:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_2);
							break;
						case 1:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE, ATM128_ADC_PRESCALE_2b);
							break;
						case 2:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_4);
							break;
						case 3:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_8);
							break;
						case 4:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_16);
							break;
						case 5:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_32);
							break;
						case 6:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_64);
							break;
						case 7:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE_128);
							break;
						case 8:
							call Atm128AdcMultiple.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,ATM128_ADC_PRESCALE);
							break;
					}
				}
			}
			if (ctrl->instr == 'g')
			{
				dataSend();	
			}
   		}
   		return message;
   	}
   	
   	async event bool Atm128AdcMultiple.dataReady(uint16_t data, bool precise,uint8_t Channel,uint8_t* NewChannel,uint8_t* NewVoltage)
	{   
		if(firstTime)
		{
			firstTime=FALSE;
			call Alarm.start(SAMPLE_PERIOD);
		}
		else if(precise)
		{
			sampleNum++;
		}
		call Leds.led2On();
		if(sampling==FALSE)
		{
			call Leds.led2Off();
		}
		return sampling;
	}
	
	async event void Alarm.fired()
	{
		sampling=FALSE;
	}
   	
	event void AMSend.sendDone(message_t* bufPtr, error_t error){}
	event void Microphone.stopDone(error_t error){}
	event void Radio.stopDone(error_t err){}
}
