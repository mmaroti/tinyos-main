configuration AccelAppC{
	
}
implementation{

	components MainC, AccelP, LedsC, SimpleFileC, RadioHandlerAppP;


	AccelP.Boot -> MainC;
	AccelP.Leds -> LedsC;

	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
	AccelP.RadioHandler -> RadioHandlerAppP;

}