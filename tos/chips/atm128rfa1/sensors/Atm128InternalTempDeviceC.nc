/**
  *
  * @author Zsolt Szabó <szabomeister@gmail.com>
  */

configuration Atm128InternalTempDeviceC {
  provides interface Read<uint16_t> as ReadTemp[uint8_t consumer];
  provides interface Resource as ResourceTemp[uint8_t consumer];
}
implementation {
  components InternalTempP;
  components new InternalTempControlP();
  components new AdcReadClientC() as Adc,
  new RoundRobinArbiterC("InternalTemp.resource") as TempArbiter;
  
  ResourceTemp = TempArbiter;
  InternalTempControlP.TempResource -> TempArbiter.Resource[unique("InternalTemp.resource")];

  ReadTemp = InternalTempControlP;
  InternalTempControlP.ActualRead -> Adc;

  Adc.Atm128AdcConfig -> InternalTempP;
}

