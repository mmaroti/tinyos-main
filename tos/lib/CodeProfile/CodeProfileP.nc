/*
* Copyright (c) 2010, University of Szeged
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
* Author: Veress Krisztian
*         veresskrisztian@gmail.com
*/

#define ATOMIC_PERIODIC_TIME 1000
//#define MAX(a,b) ((a) ^ (((a) ^ (b)) & -((a) < (b))))
//#define MIN(a,b) ((b) ^ (((a) ^ (b)) & -((a) < (b))))

#define MAX(a,b) (((a) < (b)) ? (b) : (a))
#define MIN(a,b) (((a) > (b)) ? (b) : (a))


module CodeProfileP @safe() {
  provides {
    interface StdControl;
    
    interface CodeProfile;
    interface Get<uint32_t> as MaxInterruptLength;
    interface Get<uint32_t> as MaxAtomicLength;
    interface Get<uint32_t> as MaxTaskLatency;
    
  }
  uses {
    interface Alarm<TMicro, avail_timer_width> as Alarm;
  }
}
implementation {

  uint32_t mil;           // Maximum Interrupt Length
  norace uint32_t mal, malbase;  // Maximum Atomic Length
  norace uint32_t mtl, mtlbase;  // Maximum Task Latency

  norace bool     alive;

  command uint32_t CodeProfile.getMaxInterruptLength()  { return mil; }
  command uint32_t CodeProfile.getMaxAtomicLength()     { return mal; }
  command uint32_t CodeProfile.getMaxTaskLatency()      { return mtl; }

  command uint32_t MaxInterruptLength.get() { return mil; }
  command uint32_t MaxAtomicLength.get()    { return mal; }
  command uint32_t MaxTaskLatency.get()     { return mtl; }

  command profile_t CodeProfile.getProfile() {
    profile_t ret;
    ret.max_atomic = mal;
    ret.max_interrupt = mil;
    ret.max_latency = mtl;
    return ret;
  }

  task void measureTask() {
    
    uint32_t t1 = (uint32_t)call Alarm.getNow();
    uint32_t t2 = (uint32_t)call Alarm.getNow();
    
    // The difference between two consecutive getNow() call can be
    // greater than zero, if interrupt(s) occured in between. That
    // difference is proportional to the running time of the 
    // interrupt handler.
    mil = MAX(t2-t1,mil);
    
    // The difference between the posting time of this task (mtlbase)
    // and the first expression's execution time ( t1 ) is the time
    // between two measureTask tasks.
    // This way, interleaving tasks' running time is measured.
    
    mtl = MAX(t1-mtlbase,mtl);
    atomic {
      mtlbase = (uint32_t)call Alarm.getNow();
      if ( alive )
        post measureTask();
    }
  }


  command error_t StdControl.start() {
    mil = mal = mtl = (uint32_t)0;
    mtlbase = malbase = (uint32_t)call Alarm.getNow();
    alive = TRUE;
    
    call Alarm.start(ATOMIC_PERIODIC_TIME);
    post measureTask();
    return SUCCESS;
  }
  
  command error_t StdControl.stop() {
    atomic {
      call Alarm.stop();
      alive = FALSE;
    }
    return SUCCESS;
  }
  
  async event void Alarm.fired() {
    // When this alarm should have been fired?
    uint32_t target = (malbase + ATOMIC_PERIODIC_TIME);
    
    // Get the current time AND store it for future offset
    // to avoid duplicate call to Alarm.getNow()
    malbase = (uint32_t)call Alarm.getNow();
    
    // Compute the difference, and update if necessary
    mal = MAX(MIN(malbase-target,target-malbase),mal);
       
    atomic {
      if ( alive )
        call Alarm.start(ATOMIC_PERIODIC_TIME);
    }
  }
   
}

