/*
 * Copyright (c) 2009, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Author: Janos Sallai, Miklos Maroti
 */

#include "DfrfEngine.h"

generic module DfrfClientP(uint8_t payloadLength, uint8_t uniqueLength, uint16_t bufferSize)
{
	provides
	{
		interface Init;
	}
	uses
	{
		interface DfrfControl;
	}
}

implementation
{
	uint8_t routingBuffer[sizeof(dfrf_desc_t) + bufferSize * (payloadLength + sizeof(dfrf_block_t))];

	command error_t Init.init()
	{
		/*
		 * We automatically start this buffer, since there is no point turning this buffer off
		 * because we do not reuse it for other purposes. If you are tight on memory, then
		 * you should write your own memory handling and initialize it for routing only when needed.
		 */
		return call DfrfControl.init(payloadLength, uniqueLength, routingBuffer, sizeof(routingBuffer));
	}
}
