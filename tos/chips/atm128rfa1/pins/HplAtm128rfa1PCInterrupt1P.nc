module HplAtm128rfa1PCInterrupt1P
{
  provides interface HplAtmegaPinChange;
}
implementation
{
  
// ----- pin change interrupt flag register (PCIFR)

  /* Tests if an interrupt is pending */
  async command bool HplAtmegaPinChange.test(){
    return (PCIFR&(1<<1));
  }

  /* Resets a pending interrupt */
  async command void HplAtmegaPinChange.reset(){
    PCIFR|=1<<1;
  }

// ----- pin change control register (PCICR)

  /* Enables the interrupt */
  async command void HplAtmegaPinChange.enable(){
    PCICR|=1<<1;
  }

  /* Disables the interrupt */
  async command void HplAtmegaPinChange.disable(){
    PCICR&=~(1<<1);
  }

  /* Checks if the interrupt is enabled */
  async command bool HplAtmegaPinChange.isEnabled(){
    return (PCICR&(1<<1));
  }

// ----- pin change mask register (PCMSK)

  /* Reads the mask register */
  async command uint8_t HplAtmegaPinChange.getMask(){
    return PCMSK1;
  }

  /* Sets the mask register */
  async command void HplAtmegaPinChange.setMask(uint8_t value){
    PCMSK1=value;
  }

// ----- pin register (PIN)

  /* Reads the current pin values */
  async command uint8_t HplAtmegaPinChange.getPins(){
    return PINE&1;
  }

  /* Signalled when any of the enabled pins changed */
  default event void HplAtmegaPinChange.fired(){}
  AVR_ATOMIC_HANDLER( PCINT1_vect ) {
    signal HplAtmegaPinChange.fired();
  }
}
