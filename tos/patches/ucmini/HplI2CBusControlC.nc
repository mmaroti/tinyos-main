configuration HplI2CBusControlC{
  provides interface BusControl;
}
implementation{
  components AtmegaGeneralIOC as IO, new BusControlC(TRUE, FALSE);
  #if UCMINI_REV == 49
    BusControlC.GeneralIO->IO.PortF2;
  #else
    BusControlC.GeneralIO->IO.PortF1;
  #endif
  BusControl=BusControlC;
}