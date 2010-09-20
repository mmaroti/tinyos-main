/*
* Copyright (c) 2009, University of Szeged
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

#ifndef _CODEPROFILE_H_
#define _CODEPROFILE_H_


 #if  defined(PLATFORM_IRIS)      || \
      defined(PLATFORM_BTNODE3)   || \
      defined(PLATFORM_MICA)      || \
      defined(PLATFORM_MICA2)     || \
      defined(PLATFORM_MICA2DOT)  || \
      defined(PLATFORM_MICAZ)
      
      #define MCU_IS_ATM128
      typedef uint32_t    avail_timer_width;
  #elif \
      defined(PLATFORM_EPIC)      || \
      defined(PLATFORM_EYESIFX)   || \
      defined(PLATFORM_SHIMMER)   || \
      defined(PLATFORM_SHIMMER2)  || \
      defined(PLATFORM_SHIMMER2R) || \
      defined(PLATFORM_SPAN)      || \
      defined(PLATFORM_TELOSA)    || \
      defined(PLATFORM_TELOSB)    || \
      defined(PLATFORM_TINYNODE)
      
      #define MCU_IS_MSP430
      typedef uint16_t    avail_timer_width;
      #warning "** REAL CODEPROFILE ACCURACY IS ONLY 16 BITS ON THIS PLATFORM ! **"
  #else
    #error "** THIS PLATFORM IS UNKNOWN FOR CODE PROFILING ! **"
  #endif

typedef nx_struct profile_t {
  nx_uint32_t   max_atomic;
  nx_uint32_t   max_interrupt;
  nx_uint32_t   max_latency;
} profile_t;

#endif
