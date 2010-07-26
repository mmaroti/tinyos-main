configuration AccelAppC{
	
}

implementation{

	components MainC, AccelP, LedHandlerC, SimpleFileC, RadioHandlerAppP;

	AccelP.Boot -> MainC;
	AccelP.LedHandler -> LedHandlerC;

	AccelP.SF -> SimpleFileC;
	AccelP.SFCtrl -> SimpleFileC;
	AccelP.RadioHandler -> RadioHandlerAppP;

}