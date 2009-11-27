#include "MicAdcRead.h"


configuration MicAdcReadAppC
{
}
implementation {
  
	components new AMSenderC(AM_DATA_MSG) as Send;
	components new AMReceiverC(AM_CTRL_MSG) as Receive;
	components new AlarmMicro32C() as Alarm;
	components Atm128AdcC;
	components MicAdcReadC;
	components ActiveMessageC;
	components MainC;
	components LedsC;
	components MicaBusC;
  
	MicAdcReadC.Leds -> LedsC;
	MicAdcReadC.Boot -> MainC;
	MicAdcReadC.Alarm -> Alarm;
	MicAdcReadC.Uart -> ActiveMessageC;
	MicAdcReadC.Microphone -> MicrophoneC;
	MicAdcReadC.AMSend -> Send;
	MicAdcReadC.Receive -> Receive;
	MicAdcReadC.AdcResource -> Atm128AdcC.Resource[unique(UQ_ATM128ADC_RESOURCE)];
	MicAdcReadC.Atm128AdcMultiple -> Atm128AdcC;
	MicAdcReadC.MicAdcChannel -> MicaBusC.Adc2;

	components new TimerMilliC() as MTimer;
	components new Atm128I2CMasterC() as I2CPot;
	components MicrophoneC;
 
	MicrophoneC.Timer -> MTimer;
	MicrophoneC.MicPower  -> MicaBusC.PW3;
	MicrophoneC.MicMuxSel -> MicaBusC.PW6;
	MicrophoneC.I2CResource -> I2CPot;
	MicrophoneC.I2CPacket -> I2CPot;
}
