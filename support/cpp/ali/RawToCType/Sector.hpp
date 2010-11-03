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
* Author: Ali Baharev
*/

#ifndef SECTOR_HPP_
#define SECTOR_HPP_

#include <iosfwd>

typedef unsigned short uint16;
typedef unsigned int uint32;

class sector_iterator {

public:

	explicit sector_iterator(const char* sector) : itr(sector) { }

	uint16 next_uint16();

	uint32 next_uint32();

private:

	const char* itr;
};

class header {

public:

	explicit header(sector_iterator& itr);

	uint16 sector_length() const { return length; }

	uint16 mote() const { return mote_id; }

	friend std::ostream& operator<<(std::ostream& , const header& );

private:

	uint16 format_id;
	uint16 mote_id;
	uint16 length;
};

class sample {

public:

	explicit sample(sector_iterator& itr);

	void shift_timestamp(uint32 time_start) { time_stamp -= time_start; }

	bool check_reboot(uint16 counter_previous) const;

	int missing(uint16 counter_previous) const;

	int error_in_ticks(uint32 time_previous) const;

	uint32 timestamp() const { return time_stamp; }

	uint16 counter() const { return seq_num; }

	friend std::ostream& operator<<(std::ostream& , const sample& );

private:

	uint32 time_stamp;
	uint16 seq_num;
	uint16 acc_x;
	uint16 acc_y;
	uint16 acc_z;
	uint16 gyro_x;
	uint16 gyro_y;
	uint16 gyro_z;
	uint16 volt;
	uint16 temp;
};

#endif
