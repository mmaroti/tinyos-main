module Cp2102P{
  uses {
    interface GpioPCInterrupt as Vdd;
  }
  provides interface UsbState;
  provides interface Init as PlatformInit;
}
implementation{
  
  bool Suspended;
  
  task void powerOff(){
    signal UsbState.powerOff();
  }
  task void powerOn(){
    signal UsbState.powerOn();
  }
  task void activated(){
    signal UsbState.activated();
  }
  task void suspended(){
    signal UsbState.suspended();
  }
  
  command error_t PlatformInit.init(){
    call Vdd.enable();
    return SUCCESS;
  }
  
  command bool UsbState.isActive(){
	return TRUE;
  }
  
  command bool UsbState.isPowered(){
	return call Vdd.get();
  }
  
  async event void Vdd.fired(bool toHigh){
    if(!toHigh){
      post powerOff();
    } else {
      post powerOn();
    }
  }
  
  default event void UsbState.powerOn(){}
  default event void UsbState.powerOff(){}
  default event void UsbState.activated(){}
  default event void UsbState.suspended(){}
}