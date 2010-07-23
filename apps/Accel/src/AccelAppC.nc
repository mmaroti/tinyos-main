configuration AccelAppC{
	
}
implementation{
	components MainC, AccelP, LedsC, SimpleFileC;
	AccelP.Boot -> MainC;
	AccelP.Leds -> LedsC;
	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
}