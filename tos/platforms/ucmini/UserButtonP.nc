/** Copyright (c) 2011, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Andras Biro
*/
#include <UserButton.h>
module UserButtonP{
  uses interface AtmegaPinChange as Irq;
  uses interface GeneralIO as Io;
  provides interface Init;
  provides interface Get<button_state_t>;
  provides interface Notify<button_state_t>;
}
implementation{
  
  norace bool prevState;
  
  command error_t Init.init(){
    call Io.set();
    return SUCCESS;
  }
  
  command button_state_t Get.get() { 
    if(!call Irq.get())
      return BUTTON_PRESSED;
    else
      return BUTTON_RELEASED;
  }
  
  command error_t Notify.enable(){
    if( !call Irq.isEnabled() ){//if we don't check this, the prevState can't be norace
      prevState=call Irq.get();
      call Irq.enable();
    }
    return SUCCESS;
  }
  
  command error_t Notify.disable(){
    call Irq.disable();
    return SUCCESS;
  }
  
  task void NotifyReleased(){
    signal Notify.notify( BUTTON_RELEASED );
  }
  
  task void NotifyPressed(){
    signal Notify.notify( BUTTON_PRESSED );
  }
  
  async event void Irq.fired(bool newState){
    if(prevState!=newState){
      prevState=newState;
      if(newState)
        post NotifyReleased();
      else
        post NotifyPressed();
    }
  }
}