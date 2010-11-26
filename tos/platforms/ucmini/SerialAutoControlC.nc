module SerialAutoControlC{
  uses interface SplitControl;
  uses interface UsbState;
  provides interface Init as SoftwareInit;
}
implementation{
  //TODO this component should work based on usb power (vdd) or usb active/suspend
  
  inline bool isUsbOn(){
    return call UsbState.isPowered();
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
  
  event void UsbState.powerOn(){
    post turnOn();
  }
  
  event void UsbState.powerOff(){
    post turnOff();
  }
  
  event void UsbState.activated(){
    
  }
  
  event void UsbState.suspended(){
  }
}