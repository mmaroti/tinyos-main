module SerialAutoControlC{
  uses interface SplitControl;
  uses interface GpioPCInterrupt as Vdd;
  provides interface Init as SoftwareInit;
  
  uses interface Leds;
}
implementation{
  //TODO this component should work based on usb power (vdd) or usb active/suspend
  
  inline bool isUsbOn(){
    return call Vdd.get();
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

}