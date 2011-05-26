#include "Bma180.h"

generic configuration Bma180ReadStreamClientC() {
  provides interface ReadStream<bma180_data_t>;
}
implementation {
  enum {
    ID = unique("BMA180_READSTREAM"),
    HAL_ID = uniqueCount("Atm128SpiC.Resource"),
  };

  components WireBma180StreamP, SpiImpC;
  ReadStream = WireBma180StreamP.ReadStream[ID];
  WireBma180StreamP.Resource[ID] -> SpiImpC.Resource[HAL_ID];
}
