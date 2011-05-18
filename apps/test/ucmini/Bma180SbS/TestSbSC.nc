#include "Bma180.h"
configuration TestSbSC
{
  provides interface Read<bma180_data_t>;
}

implementation
{
	components /*MainC,*/ TestSbSP, LedsC, new TimerMilliC(), DiagMsgC, SerialActiveMessageC, SpiImpC, HplAtm128GeneralIOC;

	//TestSbSP.Boot -> MainC;
  Read = TestSbSP;
	TestSbSP.Leds -> LedsC;
	TestSbSP.Timer -> TimerMilliC;
  TestSbSP.DiagMsg -> DiagMsgC;
  TestSbSP.SpiByte -> SpiImpC;
  TestSbSP.FastSpiByte -> SpiImpC;
  TestSbSP.SpiPacket -> SpiImpC;
  TestSbSP.Resource -> SpiImpC.Resource[unique("Atm128SpiC.Resource")];
  TestSbSP.CSN -> HplAtm128GeneralIOC.PortB6;
}
