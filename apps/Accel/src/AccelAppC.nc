configuration AccelAppC{
	
}
implementation{

	components MainC, AccelP, LedsC, SimpleFileC, RadioHandlerP;


	AccelP.Boot -> MainC;
	AccelP.Leds -> LedsC;

	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
	AccelP.RadioHandler -> RadioHandlerP;

}