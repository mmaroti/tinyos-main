configuration HplSi443xC
{
  provides
  {
    interface GeneralIO as NSEL;
    interface GeneralIO as IO2;
    interface GeneralIO as IO1;
    interface GeneralIO as IO0;
    interface Resource as SpiResource;
    interface FastSpiByte;
    interface GpioPCInterrupt as IRQ;
//     interface Alarm<TRadio, uint16_t> as Alarm;
//     interface LocalTime<TRadio> as LocalTimeRadio;
  }
}
implementation
{
    components HplAtm128GeneralIOC as IO, HplAtm128InterruptC as AvrIrq;
    components new NoPinC() as NoIO0, new NoPinC() as NoIO1;
    
    IRQ = AvrIrq.PCInt4;
    NSEL = IO.PortB0;
    IO0 = NoIO0;
    IO1 = NoIO1;
    IO2 = IO.PortD4;

    components Atm128SpiC as SpiC;
    SpiResource = SpiC.Resource[unique("Atm128SpiC.Resource")];
    FastSpiByte = SpiC;

//     components new AlarmOne16C() as AlarmC;
//     Alarm = AlarmC;
// 
//     components LocalTimeMicroC;
//     LocalTimeRadio = LocalTimeMicroC;
}