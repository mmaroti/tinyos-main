module I2CBusP {
  provides interface SplitControl;
  uses interface SplitControl as TemphumSplit;
  uses interface SplitControl as LightSplit;
  uses interface Splitcontrol as PressureSplit;
}
implementation {
  struct {
    unsigned int cnt:3;
  } Flag;

  Flag.cnt = 0;

  command error_t SplitControl.start() {
    call POWER.makeOutput();
    call POWER.set();

    call TemphumSplit.start();
    call LightSplit.start();
    call PressureSplit.start();
    return SUCCESS;
  }

  command error_t SplitControl.stop() {
    call POWER.clr();
    signal SplitControl.stopDone(SUCCESS);
    return SUCCESS;
  }
  
  event void TemphumSplit.startDone(error_t error) {
    Flag.cnt |= 1;
    if(Flag.cnt == 7)
      signal SplitControl.startDone(SUCCESS);
  }
  
  event void LightSplit.startDone(error_t error) {
    Flag.cnt |= 2;
    if(Flag.cnt == 7)
      signal SplitControl.starttDone(SUCCESS);
  }
  
  event void PressureSplit.startDone(error_t error) {
    Flag.cnt |= 4;
    if(Flag.cnt == 7)
      signal SplitControl.startDone(SUCCESS);
  }
  
  event void TemphumSplit.stopDone(error_t error) {
    Flag.cnt &= 6;
    if(Flag.cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }

  event void LightSplit.stopDone(error_t error) {
    Flag.cnt &= 4;
    if(Flag.cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }

  event void PressureSplit.stopDone(error_t error) {
    Flag.cnt &= 3;
    if(Flag.cnt == 0)
      signal SplitControl.stopDone(SUCCESS);
  }
  
  default event void SplitControl.startDone(error_t error) { }
  default event void SplitControl.stopDone(error_t error) { }
}
