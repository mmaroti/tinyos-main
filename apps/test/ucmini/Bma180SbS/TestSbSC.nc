configuration TestSbSC
{
}

implementation
{
	components MainC, TestSbSP, LedsC, new TimerMilliC(), DiagMsgC, SerialActiveMessageC, SpiImpC;

	TestSbSP.Boot -> MainC;
	TestSbSP.Leds -> LedsC;
	TestSbSP.Timer -> TimerMilliC;
  TestSbSP.DiagMsg -> DiagMsgC;
  TestSbSP.SpiByte -> SpiImpC;
  TestSbSP.FastSpiByte -> SpiImpC;
  TestSbSP.Resource -> SpiImpC.Resource[unique("SbS.Resource")];
}
