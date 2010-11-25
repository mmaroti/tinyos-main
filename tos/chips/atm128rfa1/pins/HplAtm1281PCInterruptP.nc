generic module HplAtm1281PCInterruptP(uint8_t ctrl_bit,  uint8_t mask_addr)
/*
 * These assumptions are used to save memory and cpu time:
 * -mask registers are in little endian format (i.e. PCINT0==PCINT8==PCINT16==0)
 */
{
  uses interface HplAtm128InterruptSig as IrqSignal;
  uses interface GeneralIO as Pin0;
  uses interface GeneralIO as Pin1;
  uses interface GeneralIO as Pin2;
  uses interface GeneralIO as Pin3;
  uses interface GeneralIO as Pin4;
  uses interface GeneralIO as Pin5;
  uses interface GeneralIO as Pin6;
  uses interface GeneralIO as Pin7;
  provides interface GpioPCInterrupt as GpioPCInterrupt0;
  provides interface GpioPCInterrupt as GpioPCInterrupt1;
  provides interface GpioPCInterrupt as GpioPCInterrupt2;
  provides interface GpioPCInterrupt as GpioPCInterrupt3;
  provides interface GpioPCInterrupt as GpioPCInterrupt4;
  provides interface GpioPCInterrupt as GpioPCInterrupt5;
  provides interface GpioPCInterrupt as GpioPCInterrupt6;
  provides interface GpioPCInterrupt as GpioPCInterrupt7;
  provides interface GpioPCInterrupt as UnknownPCInt;
}
implementation
{
  #define mask  (*TCAST(volatile uint8_t * ONE, mask_addr))
  #define flag  (*TCAST(volatile uint8_t * ONE, flag_addr))
  uint8_t state;
  uint8_t irqstate;
  bool UnknownEnabled=FALSE;
  
  inline bool pinget(uint8_t pin);
  
  
  inline error_t InterruptEnable(uint8_t pin){
	if(mask_addr&(1<<pin))
	  return EALREADY;
	else{
	  if(pinget(pin))
		state|=1<<pin;
	  mask|=1<<pin;
	  PCICR|=1<<ctrl_bit;
	  return SUCCESS;
	}
  }
  
  inline error_t InterruptDisable(uint8_t pin){
	if(!(mask_addr&(1<<pin)))
	  return EALREADY;
	else{
	  state&=~(1<<pin);
	  atomic{
		mask&=~(1<<pin);
		if(mask==0)
		  PCICR&=~(1<<ctrl_bit);
	  }
	  return SUCCESS;
	}
  }
  
  inline bool InterruptGet(uint8_t pin){
	if(mask_addr&(1<<pin)){
	  if(state&(1<<pin))
		return TRUE;
	  else
		return FALSE;
	} else {
	  return pinget(pin);
	}
  }
  
  command error_t UnknownPCInt.enable(){
	if(UnknownEnabled)
	  return EALREADY;
	else{
	  UnknownEnabled=TRUE;
	  return SUCCESS;
	}
  }
  
  command error_t UnknownPCInt.disable(){
	if(!UnknownEnabled)
	  return EALREADY;
	else{
	  UnknownEnabled=FALSE;
	  return SUCCESS;
	}
  } 
  
  command bool UnknownPCInt.get(){
	return FALSE;
  }
  
  inline void signalfired(uint8_t pin, bool toHigh);
  
  task void signalIrqs(){
	uint8_t i;
	uint8_t irqstate_sync;
	atomic irqstate_sync=irqstate;
	if(state==irqstate_sync)
	  signal UnknownPCInt.fired(FALSE);
	else {
	  for(i=0;i<8;i++){
		if( (state & (1<<i) ) != (irqstate_sync & (1<<i) ) ){
		  if(state & (1<<i) ){
			signalfired(i,FALSE);
		  } else {
			signalfired(i,TRUE);
		  }
		}
	  }
	  state=irqstate_sync;
	}
  }
  
  async event void IrqSignal.fired(){
	uint8_t i;
	irqstate=0;
	for(i=0;i<8;i++){
	  if(mask&(1<<i)){ //enabled
		if(pinget(i))
		  irqstate|=1<<i;
	  }
	}
	post signalIrqs();
  }
  

  
  inline bool pinget(uint8_t pin){
	switch(pin){
	  case 0:
		call Pin0.makeInput();
		return call Pin0.get();
	  break;
	  case 1:
		call Pin1.makeInput();
		return call Pin1.get();
	  break;
	  case 2:
		call Pin2.makeInput();
		return call Pin2.get();
	  break;
	  case 3:
		call Pin3.makeInput();
		return call Pin3.get();
	  break;
	  case 4:
		call Pin4.makeInput();
		return call Pin4.get();
	  break;
	  case 5:
		call Pin5.makeInput();
		return call Pin5.get();
	  break;
	  case 6:
		call Pin6.makeInput();
		return call Pin6.get();
	  break;
	  case 7:
		call Pin7.makeInput();
		return call Pin7.get();
	  break;
	  default:
		return FALSE;
	}
  }
  
  inline void signalfired(uint8_t pin, bool toHigh){
	switch(pin){
	  case 0:
		signal GpioPCInterrupt0.fired(toHigh);
	  break;
	  case 1:
		signal GpioPCInterrupt1.fired(toHigh);
	  break;
	  case 2:
		signal GpioPCInterrupt2.fired(toHigh);
	  break;
	  case 3:
		signal GpioPCInterrupt3.fired(toHigh);
	  break;
	  case 4:
		signal GpioPCInterrupt4.fired(toHigh);
	  break;
	  case 5:
		signal GpioPCInterrupt5.fired(toHigh);
	  break;
	  case 6:
		signal GpioPCInterrupt6.fired(toHigh);
	  break;
	  case 7:
		signal GpioPCInterrupt7.fired(toHigh);
	  break;
	}
  }
  
  command error_t GpioPCInterrupt0.enable(){
	return InterruptEnable(0);
  }
  
  command error_t GpioPCInterrupt1.enable(){
	return InterruptEnable(1);
  }
  
  command error_t GpioPCInterrupt2.enable(){
	return InterruptEnable(2);
  }
  
  command error_t GpioPCInterrupt3.enable(){
	return InterruptEnable(3);
  }
  
  command error_t GpioPCInterrupt4.enable(){
	return InterruptEnable(4);
  }
  
  command error_t GpioPCInterrupt5.enable(){
	return InterruptEnable(5);
  }
  
  command error_t GpioPCInterrupt6.enable(){
	return InterruptEnable(6);
  }
  
  command error_t GpioPCInterrupt7.enable(){
	return InterruptEnable(7);
  }  
  
  command error_t GpioPCInterrupt0.disable(){
	return InterruptDisable(0);
  }
  
  command error_t GpioPCInterrupt1.disable(){
	return InterruptDisable(1);
  }
  
  command error_t GpioPCInterrupt2.disable(){
	return InterruptDisable(2);
  }
  
  command error_t GpioPCInterrupt3.disable(){
	return InterruptDisable(3);
  }
  
  command error_t GpioPCInterrupt4.disable(){
	return InterruptDisable(4);
  }
  
  command error_t GpioPCInterrupt5.disable(){
	return InterruptDisable(5);
  }
  
  command error_t GpioPCInterrupt6.disable(){
	return InterruptDisable(6);
  }
  
  command error_t GpioPCInterrupt7.disable(){
	return InterruptDisable(7);
  }  
  
  command error_t GpioPCInterrupt0.get(){
	return InterruptGet(0);
  }
  
  command error_t GpioPCInterrupt1.get(){
	return InterruptGet(1);
  }
  
  command error_t GpioPCInterrupt2.get(){
	return InterruptGet(2);
  }
  
  command error_t GpioPCInterrupt3.get(){
	return InterruptGet(3);
  }
  
  command error_t GpioPCInterrupt4.get(){
	return InterruptGet(4);
  }
  
  command error_t GpioPCInterrupt5.get(){
	return InterruptGet(5);
  }
  
  command error_t GpioPCInterrupt6.get(){
	return InterruptGet(6);
  }
  
  command error_t GpioPCInterrupt7.get(){
	return InterruptGet(7);
  }  
  
  default event void GpioPCInterrupt0.fired(bool toHigh){}
  default event void GpioPCInterrupt1.fired(bool toHigh){}
  default event void GpioPCInterrupt2.fired(bool toHigh){}
  default event void GpioPCInterrupt3.fired(bool toHigh){}
  default event void GpioPCInterrupt4.fired(bool toHigh){}
  default event void GpioPCInterrupt5.fired(bool toHigh){}
  default event void GpioPCInterrupt6.fired(bool toHigh){}
  default event void GpioPCInterrupt7.fired(bool toHigh){}
  default event void UnknownPCInt.fired(bool toHigh){}
}