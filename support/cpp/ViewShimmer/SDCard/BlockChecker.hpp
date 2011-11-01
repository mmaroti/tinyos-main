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

#ifndef SAMPLECHECKER_HPP_
#define SAMPLECHECKER_HPP_

#include "Header.hpp"
#include "Sample.hpp"

namespace sdc {

class BlockIterator;

class BlockChecker {

public:

	explicit BlockChecker(int mote_id);

	void set_current_header(BlockIterator& i, int block_offset);

	void mote_id() const;

	bool finished() const;

	bool datalength_OK() const;

	bool reboot() const;

	unsigned int length_in_ticks() const;

	void set_current(const Sample& s);

	void shift_timestamp(Sample& s) const;

	bool time_sync_info_is_new() const;

	const Header& get_timesync() const;

private:

	BlockChecker(const BlockChecker& );

	BlockChecker& operator=(const BlockChecker& );

	void reset(int first_block);
	void check_for_new_reboot();
	void check_for_new_timesync();
	void check_first_block() const;
	void check_counter_equals_one() const;
	void check_counter() const;
	void check_timestamp() const;
	void assert_positive_time_start() const;

	const int mote_ID;

	int local_start;
	unsigned int time_start;
	bool set_time_start;
	bool new_record;
	bool new_time_sync_info;

	int block_offset;
	int samples_processed;

	Header timesync;
	Header header;

	Sample previous;
	Sample current;

};

}

#endif
