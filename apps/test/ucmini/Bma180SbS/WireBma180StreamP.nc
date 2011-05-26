#include "Bma180.h"

configuration WireBma180StreamP {
  provides interface ReadStream<bma180_data_t>[uint8_t stream];
  uses interface Resource[uint8_t stream]; 
}
implementation {
  enum {
		BMA_STREAMS = uniqueCount("BMA180_READSTREAM"),
	};

  components TestSbSC, BmaStreamP, new ArbitratedReadStreamC(BMA_STREAMS, bma180_data_t), MainC, PlatformC, new AlarmMicro32C();

  Resource   = ArbitratedReadStreamC;
  ReadStream = ArbitratedReadStreamC;

  ArbitratedReadStreamC.Service -> BmaStreamP;

  //BmaStreamP.Init <- MainC;
  BmaStreamP.Atm128Calibrate -> PlatformC;
  BmaStreamP.Read -> TestSbSC;
  //BmaStreamP.Alarm -> AlarmMicro32C;
  components DiagMsgC;
  BmaStreamP.DiagMsg -> DiagMsgC;
}
