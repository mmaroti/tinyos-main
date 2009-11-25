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
