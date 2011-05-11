module HplSi443xP
{
  provides interface GpioCapture as IRQ;
  uses interface GpioPCInterrupt as RealIRQ;
  uses interface Leds;
}
implementation
{
    task void signalIrq()
    {
      signal IRQ.captured(0);//FIXME time
    }

    async command error_t IRQ.captureFallingEdge()
    {
      call RealIRQ.enable();
      if(!call RealIRQ.get())
        post signalIrq();
      return SUCCESS;
    }

    async command error_t IRQ.captureRisingEdge()
    {
        // rising edge comes when the IRQ_STATUS register of the Si443x is read
        return FAIL;    
    }

    async command void IRQ.disable()
    {
        call RealIRQ.disable();
    }
    
    async event void RealIRQ.fired(bool toHigh){
      if(!toHigh)
        signal IRQ.captured(0);//FIXME time
    }
    
    default async event void IRQ.captured(uint16_t time){}
}