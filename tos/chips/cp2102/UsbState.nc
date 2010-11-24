interface UsbState{
   command bool isActive(); 
   command bool isPowered();
   event void powerOn();
   event void powerOff();
   event void activated();
   event void suspended();
}