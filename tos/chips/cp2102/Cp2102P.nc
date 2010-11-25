module Cp2102P{
  uses {
    interface GpioPCInterrupt as Vdd;
  }
  provides interface UsbState;
  provides interface Init as PlatformInit;
}
implementation{
  
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
  
  event void Vdd.fired(bool toHigh){
    if(!toHigh){
      signal UsbState.powerOff();
    } else {
      signal UsbState.powerOn();
    }
  }
  
  default event void UsbState.powerOn(){}
  default event void UsbState.powerOff(){}
  default event void UsbState.activated(){}
  default event void UsbState.suspended(){}
}