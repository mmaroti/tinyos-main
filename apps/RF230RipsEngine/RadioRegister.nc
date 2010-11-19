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
* Author: Miklos Maroti
*/

#include <Tasklet.h>

interface RadioRegister
{
	/**
	 * Call this function to get exclusive access to the internal registers 
	 * of the radio chip. This call returns either SUCCESS or EBUSY. You
	 * may call the write() and read() commands only if this command returns 
	 * SUCCESS. You must release the exclusive access to the radio by 
	 * calling release().
	 */
	tasklet_async command error_t access();
	
	/**
	 * This commands reads the content of an internal register of the radio 
	 * chip. This command should be bracketed between the access() and the 
	 * release() commands. Be very careful, reading some registers can 
	 * alter their content.
	 */
	tasklet_async command uint8_t read(uint8_t reg);

	/**
	 * This commands writes a value to one of the internal registers of 
	 * the radio chip. This command should be bracketed between the 
	 * access() and the release() commands. Do NOT use this command unless 
	 * you really know what you are doing.
	 */
	tasklet_async command void write(uint8_t reg, uint8_t value);

	/**
	 * Releases the exclusive access to the internal registers of the radio
	 * chip. This command must be called from the same function where
	 * access() was called, that is you canot reserve exclusive rights
	 * accross events.
	 */
	tasklet_async command void release();
}
