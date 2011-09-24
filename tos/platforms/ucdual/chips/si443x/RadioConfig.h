/*
 * Copyright (c) 2007, Vanderbilt University
 * Copyright (c) 2010, Univeristy of Szeged
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the copyright holder nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Miklos Maroti
 * Author: Krisztian Veress
 */

#include "TimerConfig.h"

#ifndef __SI443X_RADIOCONFIG_H__
#define __SI443X_RADIOCONFIG_H__

#include <Si443xDriverLayer.h>
#include "TimerConfig.h"

#ifndef SI443X_DEF_RFPOWER
#define SI443X_DEF_RFPOWER	3
#endif

#ifndef SI443X_DEF_CHANNEL
#define SI443X_DEF_CHANNEL	1
#endif

enum {
    SI443X_TXFIFO_FULL_THRESH = 55,
    SI443X_RXFIFO_FULL_THRESH = 55,
    SI443X_TXFIFO_EMPTY_THRESH = 4,
};


#if ( SI443X_TXFIFO_FULL_THRESH >= 64 || SI443X_TXFIFO_FULL_THRESH < 0 )
#error "SI443X_TXFIFO_FULL_THRESH value is not valid!"
#endif

#if ( SI443X_TXFIFO_EMPTY_THRESH >= 64 || SI443X_TXFIFO_EMPTY_THRESH < 0 )
#error "SI443X_TXFIFO_EMPTY_THRESH value is not valid!"
#endif

#if ( SI443X_TXFIFO_EMPTY_THRESH > SI443X_TXFIFO_FULL_THRESH )
#error "SI443X_TXFIFO_FULL_THRESH cannot be lesser than SI443X_TXFIFO_EMPTY_THRESH!"
#endif

#if ( SI443X_RXFIFO_FULL_THRESH >= 64 || SI443X_RXFIFO_FULL_THRESH < 0 )
#error "SI443X_RXFIFO_FULL_THRESH value is not valid!"
#endif

// This is the timer type of the radio alarm interface
typedef T62khz TRadio;
typedef uint32_t tradio_size;

// The number of radio alarm ticks per one microsecond
#define RADIO_ALARM_MICROSEC	0.0625

// The base two logarithm of the number of radio alarm ticks per one millisecond
#define RADIO_ALARM_MILLI_EXP	6

#endif//__SI443X_RADIOCONFIG_H__
