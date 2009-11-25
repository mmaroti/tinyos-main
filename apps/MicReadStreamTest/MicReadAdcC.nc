#include "MicReadAdc.h"

module MicReadAdcC
{
	uses {
		interface Alarm<TMicro, uint32_t>;
		interface Leds;
		interface Boot;
		interface Receive;
		interface AMSend;
		interface SplitControl as Uart;
		interface Resource as AdcResource;
		interface Atm128AdcSingle;
		interface MicaBusAdc as MicAdcChannel;
		interface SplitControl as Microphone;
  	}
}
implementation {
  
	enum
	{
		IDLE=0,
		SAMPLING,
		SENDING,
	};
  
  	norace uint8_t state = IDLE;
  	norace uint8_t preScale;
	norace uint32_t sampleNum;
	norace bool firstTime;
	message_t msg;
	
	inline void dataSend()
	{
		data_msg_t* packet = (data_msg_t*)(call AMSend.getPayload(&msg, sizeof(data_msg_t)));
    	packet-> sampleNum = sampleNum;
	   	call AMSend.send(AM_BROADCAST_ADDR, &msg, sizeof(data_msg_t));
//	   	call Leds.led2Toggle();
    }

    task void stopMicrophone()
    {
    	call Microphone.stop();
    }
	  	
	event void Boot.booted()
	{
		call Uart.start();
//		call Leds.led0On();
	}
  
	event void Uart.startDone(error_t err)
	{
		if(err==SUCCESS)
		{
			if (state == SENDING)
			{
				dataSend();
			}
		}
		else
		{
			call Uart.start();
		}
	}
	
	event void Uart.stopDone(error_t err)
   	{
   		if(err==SUCCESS)
   		{
			call AdcResource.request();
   		}
   		else
   		{
   			call Uart.stop();
   		}
   	}
   	
   	event void AdcResource.granted()
  	{
		call Microphone.start();
	}
	
	event void Microphone.startDone(error_t error)
  	{
    	if (error == SUCCESS)
    	{
    		state = SAMPLING;
    		call Atm128AdcSingle.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,preScale);
    	}
    	else
    	{
    		call Microphone.start();
    	}
   	}
   	
   	async event void Atm128AdcSingle.dataReady(uint16_t data, bool precise)
	{   
		if (state == SAMPLING)
		{
			call Atm128AdcSingle.getData(call MicAdcChannel.getChannel(),ATM128_ADC_VREF_OFF,FALSE,preScale);
		}
		if(firstTime)
		{
			call Alarm.start(SAMPLE_PERIOD);
			firstTime=FALSE;
		}
		else if(precise)
		{
			sampleNum++;
		}
	}
	
	async event void Alarm.fired()
	{
		state = SENDING;
		call AdcResource.release();
		post stopMicrophone();
	}
	
	event void Microphone.stopDone(error_t error)
	{
		if (error == SUCCESS)
		{
			call Uart.start();
		}
		else
		{
			call Microphone.stop();
		}
	}
   		
	event message_t* Receive.receive(message_t* message ,void* payload, uint8_t len)
	{
		if (len == sizeof(ctrl_msg_t))
		{
			ctrl_msg_t *ctrl = (ctrl_msg_t*)payload;
			if (ctrl->instr == 's')
			{
				preScale=ctrl->preScale;
				sampleNum=0;
				firstTime=TRUE;
//				call Leds.led1On();
				call Uart.stop();
			}
   		}
   		return message;
   	}
   	
	event void AMSend.sendDone(message_t* bufPtr, error_t error)
	{
	}
}
