generic configuration ReadClientC()
{
  provides {
    interface Read<uint16_t>;
  }
}
implementation
{
  #define UQ_BMA180_RESOURCE "Bma180.ReadResource"
  components new ArbitratedReadC(uint16_t), new FcfsArbiterC(UQ_BMA180_RESOURCE),
             new StdControlPowerManagerC(), ReadClientP, TestSbSC;
  
  Read=ArbitratedReadC[unique(UQ_BMA180_RESOURCE)];
  
  StdControlPowerManagerC.ResourceDefaultOwner -> FcfsArbiterC;
  StdControlPowerManagerC.Arbiterinfo -> FcfsArbiterC;
  StdControlPowerManagerC.StdControl-> TestSbSC
  
  ArbitratedReadC.Resource=FcfsArbiterC;
  ArbitratedReadC.Service=ReadClientP;
  
  ReadClientP.ActualRead->TestSbSC;
}