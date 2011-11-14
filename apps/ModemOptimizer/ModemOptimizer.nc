/*
* Copyright (c) 2011, University of Szeged
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

#include <Tasklet.h>

interface ModemOptimizer {

	/** Reset the internal driver variables to make the measurement always fair. */
	command void reset();
	
	/** Configure the driver to the modem configuration numbered 'id' */
	command void configure(uint32_t id);
	
	tasklet_async event void configured();
	
	/** Ask the driver how many test should we perform */
	command uint32_t configCount();
		
	/** Different events from the driver */
	tasklet_async event void sendRequest();
	tasklet_async event void sendBusy();
	tasklet_async event void sendAccept();
	tasklet_async event void sendError();
	tasklet_async event void sendSuccess();
	tasklet_async event void receiveSync();
	tasklet_async event void receiveError();
	tasklet_async event void receiveCRC();
	tasklet_async event void receiveSuccess();
}
