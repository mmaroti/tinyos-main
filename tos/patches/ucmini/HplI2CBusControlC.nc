configuration HplI2CBusControlC{
  provides interface BusControl;
}
implementation{
  components HplAtm128GeneralIOC, new BusControlC(TRUE, FALSE);
  #if UCMINI_REV == 49
    BusControlC.GeneralIO->HplAtm128GeneralIOC.PortF2;
  #else
    BusControlC.GeneralIO->HplAtm128GeneralIOC.PortF1;
  #endif
  BusControl=BusControlC;
}