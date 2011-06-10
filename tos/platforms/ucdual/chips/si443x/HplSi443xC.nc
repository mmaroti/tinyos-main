configuration HplSi443xC
{
  provides
  {
    interface GeneralIO as NSEL;
    interface GeneralIO as SDN;
    
    interface GeneralIO as IO2;
    interface GeneralIO as IO1;
    interface GeneralIO as IO0;
    interface Resource as SpiResource;
    interface FastSpiByte;
    interface GpioCapture as IRQ;
    interface Alarm<TRadio, tradio_size> as Alarm;
    interface LocalTime<TRadio> as LocalTimeRadio;
  }
}
implementation
{
    components HplAtm128GeneralIOC as IO, HplAtm128InterruptC as AvrIrq, HplSi443xP;
    components new NoPinC() as NoIO0, new NoPinC() as NoIO1;
    
    NSEL = IO.PortF0;
    SDN = NoIO0;
    
    
    IRQ = HplSi443xP.IRQ;
    HplSi443xP.RealIRQ->AvrIrq.PCInt4;
    
    IO0 = NoIO0;
    IO1 = NoIO1;
    IO2 = IO.PortD4;

    components Atm128SpiC as SpiC;
    SpiResource = SpiC.Resource[unique("Atm128SpiC.Resource")];
    FastSpiByte = SpiC;

    components new AlarmMcu16C() as AlarmC;
    Alarm = AlarmC;
 
    components LocalTimeMcuC;
    LocalTimeRadio = LocalTimeMcuC;
}
