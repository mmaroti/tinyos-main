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
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "SDCardImpl.hpp"
#include "BlockDevice.hpp"
#include "Tracker.hpp"
#include "BlockIterator.hpp"
#include "Header.hpp"
#include "BlockRelatedConsts.hpp"
#include "Constants.hpp"

using namespace std;

// FIXME Understand why crashes if offset is incorrect
// TODO Introduce new datamembers to write metadata to flat-file db
// TODO Needs explicit closing of out

SDCardImpl::SDCardImpl(BlockDevice* source)
	: device(source), out(new ofstream()), tracker(0)
{
	out->exceptions(ofstream::failbit | ofstream::badbit);
	time_start = 0;
	time_previous = 0;
	counter_previous = 1;
	samples_processed = 0;
	mote_id = -1;
	block_offset = 0;
	reboot_seq_num = 0;

	set_mote_id();

	tracker = new Tracker(mote_id);
}

void SDCardImpl::set_mote_id() {

	const char* const block = device->read_block(0);

	if (block==0) {

		clog << "Error: failed to read the first block ";
		clog << "when looking for the mote ID, exiting..." << endl;
		exit(1);
	}

	BlockIterator itr(block);

	header h(itr);

	mote_id = h.mote();
}

void SDCardImpl::process_new_measurements() {

	bool finished = false;

	const char* block = 0;

	block_offset = tracker->start_from_here();

	reboot_seq_num = tracker->reboot();

	while ((block = device->read_block(block_offset)) && !finished ) {

		finished = process_block(block);

		++block_offset;
	}
}

SDCardImpl::~SDCardImpl() {

	delete device;
	delete out;
	delete tracker;
}

void SDCardImpl::create_new_file() {

	ostringstream os;

	os << 'm' << setfill('0') << setw(3) << mote_id << '_';
	os << 'r' << setfill('0') << setw(3) << reboot_seq_num << '_';
	os << 's' << block_offset << ".csv" << flush;

	if (out->is_open())
		out->close();

	out->open(os.str().c_str());
}

bool SDCardImpl::reboot(const int sample_in_block) {

	bool reboot = s.check_reboot(counter_previous);

	if (reboot && sample_in_block==0) {

		cout << "Found a reboot at sample " << samples_processed << endl;

		++reboot_seq_num;

		time_start = s.timestamp();

		create_new_file();

		return true;
	}

	return false;
}

void SDCardImpl::check_counter() const {

	int missed = s.missing(counter_previous);

	if (missed) {

		clog << "Warning: at sample " << samples_processed;
		clog << " missing at least ";
		clog << missed << " samples" << endl;
	}
}

void SDCardImpl::check_timestamp() const {

	int dt = s.error_in_ticks(time_previous);

	if (abs(dt)>TOLERANCE) {

		clog << "Warning: at sample " << samples_processed << " ";
		clog << dt << " ticks error" << endl;
	}
}

void SDCardImpl::check_sample(const int sample_in_block) {

	if (!reboot(sample_in_block)) {

		check_counter();

		check_timestamp();
	}

	time_previous = s.timestamp();

	counter_previous = s.counter();
}

void SDCardImpl::write_samples(BlockIterator& itr) {

	for (int i=0; i<MAX_SAMPLES; ++i) {

		s = sample(itr);

		check_sample(i);

		s.shift_timestamp(time_start);

		*out << s;

		++samples_processed;
	}

	*out << flush;
}

void SDCardImpl::check_mote_id(int id) const {

	if (id != mote_id) {

		clog << "Warning: mote id " << id << " in block " << block_offset;
		clog << " does differs from " << mote_id << endl;
	}
}

bool SDCardImpl::check_data_length(int length) const {

	bool is_ok = true;

	if (MAX_SAMPLES != length/SAMPLE_LENGTH) {

		clog << "Warning: invalid length in block " << block_offset << endl;
		is_ok = false;
	}

	return is_ok;
}

bool SDCardImpl::process_block(const char* block) {

	BlockIterator itr(block);

	const header h(itr);

	const int length = h.data_length();

	if (length == 0) {

		cout << "Finished!" << endl;
		return true;
	}

	if (check_data_length(length)) {

		check_mote_id( h.mote() );
		write_samples(itr);
	}

	return false;
}
