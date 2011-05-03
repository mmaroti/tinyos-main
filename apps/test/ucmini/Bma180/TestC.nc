configuration TestC
{
}

implementation
{
	components MainC, TestP, LedsC, new TimerMilliC();

	TestP.Boot -> MainC;
	TestP.Leds -> LedsC;
	TestP.Timer -> TimerMilliC;
}
