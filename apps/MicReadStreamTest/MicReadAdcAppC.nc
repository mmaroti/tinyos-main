#include "MicReadAdc.h"


configuration MicReadAdcAppC
{
}
implementation {
  
	components new AMSenderC(AM_DATA_MSG) as Send;
	components new AMReceiverC(AM_CTRL_MSG) as Receive;
	components new AlarmMicro32C() as Alarm;
	components Atm128AdcC;
	components MicReadAdcC;
	components ActiveMessageC;
	components MainC;
	components LedsC;
	components MicaBusC;
  
	MicReadAdcC.Leds -> LedsC;
	MicReadAdcC.Boot -> MainC;
	MicReadAdcC.Alarm -> Alarm;
	MicReadAdcC.Uart -> ActiveMessageC;
	MicReadAdcC.Microphone -> MicrophoneC;
	MicReadAdcC.AMSend -> Send;
	MicReadAdcC.Receive -> Receive;
	MicReadAdcC.AdcResource -> Atm128AdcC.Resource[unique(UQ_ATM128ADC_RESOURCE)];
	MicReadAdcC.Atm128AdcSingle -> Atm128AdcC;
	MicReadAdcC.MicAdcChannel -> MicaBusC.Adc2;

	components new TimerMilliC() as MTimer;
	components new Atm128I2CMasterC() as I2CPot;
	components MicrophoneC;
 
	MicrophoneC.Timer -> MTimer;
	MicrophoneC.MicPower  -> MicaBusC.PW3;
	MicrophoneC.MicMuxSel -> MicaBusC.PW6;
	MicrophoneC.I2CResource -> I2CPot;
	MicrophoneC.I2CPacket -> I2CPot;
	MicrophoneC.Leds -> LedsC;
}
