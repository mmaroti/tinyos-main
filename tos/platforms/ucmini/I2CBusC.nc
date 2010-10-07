configuration I2CBusC {
  provides interface SplitControl as BusControl;
  
  uses interface GeneralIO as POWER;
  uses interface SplitControl as TemphumSplit;
  uses interface SplitControl as LightSplit;
  uses interface Splitcontrol as PressureSplit;
}
implementation {
  components HplAtm128GeneralIOC, I2CBusP;
  components new HplAtm128GeneralIOPinP() as PWRM;

  PWRM -> HplAtm128GeneralIOC.PortF2;
  BusControl = I2CBusP.SplitControl;

  TemphumSplit = I2CBusP.TemphumSplit;
  LightSplit   = I2CBusP.LightSplit;
  PressureSplit= I2CBusP.PressureSplit;

  components Sht21P, bh1750P, ms5607P;
  TemphumSplit -> Sht21P.SplitControl;
  LightSplit   -> bh1750P.SplitControl;
  PressureSplit-> ms5607P.SplitControl;
}
