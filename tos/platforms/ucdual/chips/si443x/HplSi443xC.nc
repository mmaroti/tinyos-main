#include <RadioConfig.h>

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
    interface GpioInterrupt as IRQ;
    interface Alarm<TRadio, tradio_size> as Alarm;
    interface LocalTime<TRadio> as LocalTimeRadio;
  }
}
implementation
{
    components HplAtm128GeneralIOC as IO, new NoPinC();
    NSEL = IO.PortF0;
    SDN = NoPinC;
    IO0 = NoPinC;
    IO1 = NoPinC;
    IO2 = IO.PortD4;
    
    components AtmegaPinChange0C;
    IRQ = AtmegaPinChange0C.GpioInterrupt[4];

    components Atm128SpiC as SpiC;
    SpiResource = SpiC.Resource[unique("Atm128SpiC.Resource")];
    FastSpiByte = SpiC;

    components new Alarm62khz32C() as AlarmC;
    Alarm = AlarmC;
 
    components LocalTime62khzC as LocalTimeC;
    LocalTimeRadio = LocalTimeC;
}
