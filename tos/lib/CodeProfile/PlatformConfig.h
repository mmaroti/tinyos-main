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
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/

#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

// MicroController Identification

#if   defined(PLATFORM_IRIS)      || defined(PLATFORM_BTNODE3)    || \
      defined(PLATFORM_MICA)      || defined(PLATFORM_MICA2)      || \
      defined(PLATFORM_MICA2DOT)  || defined(PLATFORM_MICAZ)
      
      #define CONTROLLER_ATM128
      
#elif defined(PLATFORM_EPIC)      || defined(PLATFORM_EYESIFX)    || \
      defined(PLATFORM_EYESIFXV1) || defined(PLATFORM_EYESIFXV2)  || \
      defined(PLATFORM_SHIMMER)   || defined(PLATFORM_SHIMMER2)   || \
      defined(PLATFORM_SHIMMER2R) || defined(PLATFORM_SPAN)       || \
      defined(PLATFORM_TELOSA)    || defined(PLATFORM_TELOSB)     || \
      defined(PLATFORM_TINYNODE)
      
      #define CONTROLLER_MSP430

#elif defined(PLATFORM_INTELMOTE2)

      #define CONTROLLER_PXA27X
      
#elif defined(PLATFORM_MULLE)

      #define CONTROLLER_M16C62P

#else
      #define CONTROLLER_UNKNOWN
#endif // MicroController Identification

// Radio Chip identification

#if   defined(PLATFORM_IRIS)      || defined(PLATFORM_MULLE)      
      
      #define RADIO_RF230
      
#elif defined(PLATFORM_MICA2)     || defined(PLATFORM_MICA2DOT)   || \
      defined(PLATFORM_BTNODE3)   || defined(PLATFORM_MICA)
      
      #define RADIO_CC1000

#elif defined(PLATFORM_MICAZ)     || defined(PLATFORM_SHIMMER)    || \
      defined(PLATFORM_SHIMMER2)  || defined(PLATFORM_SHIMMER2R)  || \
      defined(PLATFORM_TELOSA)    || defined(PLATFORM_TELOSB)     || \
      defined(PLATFORM_EPIC)      || defined(PLATFORM_EPIC)       || \
      defined(PLATFORM_SPAN)

      #define RADIO_CC2420

#elif defined(PLATFORM_EYESIFX)   || defined(PLATFORM_EYESIFXV1)  || \
      defined(PLATFORM_EYESIFXV2)

      #define RADIO_TDA5250

#elif defined(PLATFORM_TINYNODE)
      
      #define RADIO_XE1205
      
#else
      #define RADIO_UNKNOWN
#endif // Radio Chip Indentification


#endif
