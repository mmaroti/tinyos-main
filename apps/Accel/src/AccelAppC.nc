configuration AccelAppC{
	
}
implementation{
	components MainC, AccelP, LedsC;
	AccelP.Boot -> MainC.Boot;
	AccelP.Leds -> LedsC.Leds;
}