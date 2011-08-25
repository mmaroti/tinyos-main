generic module PinChange2GpioInt
{
  provides interface GpioInterrupt as IRQ;
  uses interface AtmegaPinChange as RealIRQ;
}
implementation
{
  typedef struct state_t {
    bool prevState          : 1;
    bool risingEdgeEnabled  : 1;
    bool fallingEdgeEnabled : 1;
  } state_t; 
  state_t state={FALSE, FALSE, FALSE};
  
  async command error_t IRQ.enableFallingEdge()
  {
    if(!state.risingEdgeEnabled && !state.fallingEdgeEnabled){
      state.prevState=call RealIRQ.get();
      call RealIRQ.enable();
    } 
    state.risingEdgeEnabled=FALSE;
    state.fallingEdgeEnabled=TRUE;
    return SUCCESS;
  }

  async command error_t IRQ.enableRisingEdge()
  {
    if(!state.risingEdgeEnabled && !state.fallingEdgeEnabled){
      state.prevState=call RealIRQ.get();
      call RealIRQ.enable();
    }
    state.fallingEdgeEnabled=FALSE;
    state.risingEdgeEnabled=TRUE;
    return SUCCESS;    
  }

  async command void IRQ.disable()
  {
    state.fallingEdgeEnabled=FALSE;
    state.risingEdgeEnabled=FALSE;
    call RealIRQ.disable();
  }
  
  async event void RealIRQ.fired(bool newState){
    if(state.prevState!=newState){
      state.prevState=newState
      if(state.risingEdgeEnabled && newState){
        signal IRQ.fired();
      } else if(state.fallinEdgeEnabled && !newState){
        signal IRQ.fired();
      }
    }
  }
  
  default async event void IRQ.fired(){}
}