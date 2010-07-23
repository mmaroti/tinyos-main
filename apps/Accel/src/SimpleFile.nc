/** Copyright (c) 2010, University of Szeged
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

interface SimpleFile
{
	/**
	 * Erases all packets.
	 */
	command error_t format();

	/**
	 * Signals after the completion of the format command
	 */
	event void formatDone(error_t error);

	/**
	 * The total number of packets
	 */
	command uint32_t size();

	/**
	 * Adds a new packet of size length at the end of the list of packets
	 */
	command error_t append(uint8_t *packet, uint16_t length);

	event void appendDone(error_t error);

	/**
	 * Moves the read pointer to the give packet index, which
	 * must be between 0 and count-1.
	 */
	command error_t seek(uint32_t index);

	/**
	 * Returns the length of the packet at the given index.
	 */
	event void seekDone(error_t error);

	/**
	 * Reads at most length many bytes into the given buffer.
	 */
	command error_t read(uint8_t *packet, uint16_t length);

	/**
	 * Returns the length of the read packet.
	 */
	event void readDone(error_t error, uint16_t length);
}
