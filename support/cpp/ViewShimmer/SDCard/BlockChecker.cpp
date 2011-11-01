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

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "BlockChecker.hpp"
#include "BlockIterator.hpp"
#include "Constants.hpp"
#include "BlockRelatedConsts.hpp"

using std::cout;
using std::endl;
using std::logic_error;
using std::abs;

namespace sdc {

void fix_counter_overflow(int& i) {

	if (i < 0)
		i += 0x10000;
}

BlockChecker::BlockChecker(int mote_id) : mote_ID(mote_id) {

	reset(-1);
	time_start = 0;
	block_offset = -1;
}

void BlockChecker::reset(int first_block) {

	local_start = first_block;
	set_time_start = false;
	new_record = false;
	new_time_sync_info = false;
	timesync.set_timesync_zero();
	samples_processed = 0;
}

bool BlockChecker::reboot() const {

	return new_record;
}

void BlockChecker::set_current_header(BlockIterator& i, int offset) {

	header = Header(i);

	block_offset = offset;

	if (!finished()) {

		check_for_new_reboot();

		check_for_new_timesync();
	}
}

bool BlockChecker::time_sync_info_is_new() const {

	return new_time_sync_info;
}

const Header& BlockChecker::get_timesync() const {

	return timesync;
}

void BlockChecker::check_for_new_reboot() {

	const int header_first_block = static_cast<int> (header.first_block());

	if (local_start != header_first_block) {

		reset(header_first_block);

		new_record = true;

		set_time_start = true;

		check_first_block();
	}
	else {

		new_record = false;
	}
}

void BlockChecker::check_first_block() const {

	if (local_start != block_offset) {

		cout << "Error: found a new record at block " << block_offset;
		cout << " but the block field in the header is " << local_start << endl;

		throw std::runtime_error("corrupted data on SD card (forgot to format?)");
	}
}

void BlockChecker::check_for_new_timesync() {

	if ( header.timesync_differs_from(timesync) ) {

		timesync = header;

		new_time_sync_info = true;
	}
	else {

		new_time_sync_info = false;
	}
}

void BlockChecker::mote_id() const {

	if (header.mote() != mote_ID) {

		cout << "Warning: mote id " << header.mote() << " in block ";
		cout << block_offset << " differs from " << mote_ID << endl;
	}
}

bool BlockChecker::finished() const {

	return header.data_length()==0;
}

bool BlockChecker::datalength_OK() const {

	bool is_ok = true;

	if (MAX_SAMPLES != header.data_length()/SAMPLE_LENGTH) {

		cout << "Warning: invalid length " << header.data_length();
		cout << " in block " << block_offset << endl;

		is_ok = false;
	}

	return is_ok;
}

void BlockChecker::set_current(const Sample& s) {

	previous = current;

	current = s;

	if (set_time_start) {

		set_time_start = false;

		time_start = s.timestamp();

		check_counter_equals_one();
	}
	else {

		check_counter();

		check_timestamp();
	}

	++samples_processed;
}

void BlockChecker::check_counter_equals_one() const {

	const unsigned short counter = current.counter();

	if (counter!=1) {

		cout << "Warning: counter should be 1 but it is " << counter << " in ";
		cout << "block " << block_offset << endl;
	}
}

void BlockChecker::check_counter() const {

	int diff = current.counter()-previous.counter();

	fix_counter_overflow(diff);

	int missed = diff-1;

	if (missed) {

		cout << "Warning: at sample " << samples_processed;
		cout << " missing at least ";
		cout << missed << " samples" << endl;
	}
}

void BlockChecker::check_timestamp() const {

	uint32 expected = previous.timestamp() + SAMPLING_RATE;

	int error_in_ticks = current.timestamp() - expected;

	if (abs(error_in_ticks)>TOLERANCE) {

		cout << "Warning: at sample " << samples_processed << " ";
		cout << error_in_ticks << " ticks error" << endl;
	}
}

void BlockChecker::assert_positive_time_start() const {

	if (time_start == 0) {

		throw logic_error("time start not set");
	}
}

void BlockChecker::shift_timestamp(Sample& s) const {

	assert_positive_time_start();

	s.shift_timestamp(time_start);
}

unsigned int BlockChecker::length_in_ticks() const {

	assert_positive_time_start();

	return current.timestamp()-time_start;
}

}

