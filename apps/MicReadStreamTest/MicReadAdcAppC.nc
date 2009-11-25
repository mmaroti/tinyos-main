#include "MicReadAdc.h"


configuration MicReadAdcAppC
{
}
implementation {
  
	components new AlarmMilli32C() as Alarm;
	components Atm128AdcC;
	components MicReadAdcC;
	components MainC;
	components LedsC;
	components MicaBusC;
  
	MicReadAdcC.Leds -> LedsC;
	MicReadAdcC.Boot -> MainC;
	MicReadAdcC.Alarm -> Alarm;
	MicReadAdcC.Microphone -> MicrophoneC;
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
