module SerialAutoControlC{
  uses interface SplitControl;
  uses interface GpioPCInterrupt as Vdd;
	#if (UCMINI_REV != 49)
	uses interface GpioPCInterrupt as NSuspend;
	#endif
  provides interface Init as SoftwareInit;
  provides interface SplitControl as DummyControl;
}
implementation{
  #define SERIAL_AUTO_NSUS
  //check revision number
  #if defined(SERIAL_AUTO_NSUS) && UCMINI_REV==49
	#error "SERIAL_AUTO_NSUS is unsopportet on UCMini 0.49 (UCMINI_REV=49)"
  #endif
  
  inline bool isUsbOn(){
	#ifdef SERIAL_AUTO_NSUS
	  return call NSuspend.get();
	#else
	  return call Vdd.get();
	#endif
  }
  
  task void turnOn(){
    error_t err=call SplitControl.start();  
    if(err!=SUCCESS&&err!=EALREADY)
      post turnOn();
  }
  
  task void turnOff(){
    error_t err=call SplitControl.stop();
    if(err!=SUCCESS&&err!=EALREADY)
      post turnOff();
  }
  
  command error_t SoftwareInit.init(){
    if(isUsbOn()){
      post turnOn();
    }
    #ifdef SERIAL_AUTO_NSUS
	  call NSuspend.enable();
	#else
	  call Vdd.enable();
	#endif
    return SUCCESS;
  }
  
  event void SplitControl.startDone(error_t err){
    if(err!=SUCCESS)
      call SplitControl.start();
  }
  
  event void SplitControl.stopDone(error_t err){
    if(err!=SUCCESS)
      call SplitControl.stop();
  }
  
  async event void Vdd.fired(bool toHigh){
      if(toHigh)
	post turnOn();
      else
	post turnOff();
  }
  
  #if (UCMINI_REV != 49)
  async event void NSuspend.fired(bool toHigh){
      if(toHigh)
	post turnOn();
      else
	post turnOff();
  }
  #endif
  
  //TODO maybe we should enable real manual control here, but now it's just a dummy interface providing for backward compatibility
  
  task void dummyStart(){
    signal DummyControl.startDone(SUCCESS);
  }
  
  task void dummyStop(){
    signal DummyControl.stopDone(SUCCESS);
  }
  
  command error_t DummyControl.start(){
    post dummyStart();
    return SUCCESS;
  }
  
  command error_t DummyControl.stop(){
    post dummyStop();
    return SUCCESS;
  }

  default event void DummyControl.startDone(error_t err){}
  default event void DummyControl.stopDone(error_t err){}

}