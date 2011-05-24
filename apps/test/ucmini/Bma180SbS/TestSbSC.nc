#include "Bma180.h"
configuration TestSbSC
{
  provides interface SplitControl;
  provides interface Read<bma180_data_t>;
  //provides interface ReadStream<bma180_data_t>;
}

implementation
{
	components /*MainC,*/ TestSbSP, /*new Bma180ReadStreamClientC(),*/ LedsC, new TimerMilliC(), DiagMsgC, SerialActiveMessageC, SpiImpC, HplAtm128GeneralIOC;

	//TestSbSP.Boot -> MainC;
  SplitControl = TestSbSP.BmaControl;
  Read = TestSbSP;
  //ReadStream = Bma180ReadStreamClientC;
	TestSbSP.Leds -> LedsC;
	TestSbSP.Timer -> TimerMilliC;
  TestSbSP.DiagMsg -> DiagMsgC;
  TestSbSP.SpiByte -> SpiImpC;
  TestSbSP.FastSpiByte -> SpiImpC;
  TestSbSP.SpiPacket -> SpiImpC;
  TestSbSP.Resource -> SpiImpC.Resource[unique("Atm128SpiC.Resource")];
  TestSbSP.CSN -> HplAtm128GeneralIOC.PortB6;
  TestSbSP.PWR -> HplAtm128GeneralIOC.PortF0;
}
