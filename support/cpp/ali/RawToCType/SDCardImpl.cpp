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
#include <stdexcept>
#include "SDCardImpl.hpp"
#include "BlockDevice.hpp"
#include "BlockIterator.hpp"
#include "BlockRelatedConsts.hpp"
#include "BlockChecker.hpp"
#include "Constants.hpp"
#include "Tracker.hpp"

using namespace std;

// FIXME Understand why crashes if offset is incorrect

SDCardImpl::SDCardImpl(BlockDevice* source)
	: device(source), out(new ofstream()), tracker(0), check(0)
{
	out->exceptions(ofstream::failbit | ofstream::badbit);

	time_start = 0;

	block_offset = 0;

	reboot_seq_num = 0;

	init_tracker();

	check.reset(new BlockChecker(tracker->mote_id()));
}

void SDCardImpl::init_tracker() {

	const char* const block = device->read_block(0);

	if (block==0) {
		throw runtime_error("Failed to read the zeroth block when looking for the mote ID");
	}

	BlockIterator zeroth_block(block);

	tracker.reset(new Tracker(zeroth_block));
}

double SDCardImpl::size_GB() const {

	return device->size_GB();
}

void SDCardImpl::print_start_banner() const {

	cout << "Card size is ";
	cout << setprecision(2) << fixed << device->size_GB() << endl;
	cout << "Starting at block " << block_offset << ", ";
	cout << "previous reboot sequence number is " << reboot_seq_num << endl;
}

void SDCardImpl::print_finished_banner() const {

	const unsigned int GB = 1 << 30;

	const double used_bytes = block_offset*BLOCK_SIZE ;

	const double used_GB = used_bytes/GB;

	const double remaining = device->size_GB()-used_GB;

	const double remaining_blocks = (remaining/BLOCK_SIZE*GB);

	const double remaining_samples = remaining_blocks*MAX_SAMPLES;

	const double remaining_sec = (remaining_samples*SAMPLING_RATE)/TICKS_PER_SEC;

	cout << "Remaining " << remaining << " GB, approximately ";
	cout << setprecision(2) << fixed << remaining_sec/3600 << " hours" << endl;
	cout << "Finished!" << endl;
}

void SDCardImpl::process_new_measurements() {

	bool finished = false;

	const char* block = 0;

	block_offset = tracker->start_from_here();

	reboot_seq_num = tracker->reboot();

	print_start_banner();

	while ((block = device->read_block(block_offset)) && !finished ) {

		finished = process_block(block);

		++block_offset;
	}

	close_out_if_open();

	print_finished_banner();
}

void SDCardImpl::close_out_if_open() {

	if (out->is_open()) {

		uint32 length_in_ticks = check->get_previous_timestamp()-time_start;

		tracker->append_to_db(block_offset-1, length_in_ticks);

		out->close();
	}
}

void SDCardImpl::create_new_file() {

	ostringstream os;

	os << 'm' << setfill('0') << setw(3) << tracker->mote_id() << '_';
	os << 'r' << setfill('0') << setw(3) << reboot_seq_num << '_';
	os << 's' << block_offset << ".csv" << flush;

	out->open(os.str().c_str());

	tracker->mark_beginning(block_offset, reboot_seq_num);
}

bool SDCardImpl::reboot(const int sample_in_block) {

	bool reboot = check->reboot();

	if (reboot && sample_in_block==0) {

		close_out_if_open();

		++reboot_seq_num;

		cout << "Reboot " << reboot_seq_num << " at processed sample ";
		cout << check->processed() << endl;

		time_start = check->get_current_timestamp();

		create_new_file();

		return true;
	}
	// FIXME Should not we log if sample_in_block != 0 ?

	return false;
}

void SDCardImpl::check_sample(const int sample_in_block) {

	// TODO May this should be pushed into BlockChecker
	if (!reboot(sample_in_block)) {

		check->counter();

		check->timestamp();
	}
}

void SDCardImpl::write_samples(BlockIterator& itr) {

	for (int i=0; i<MAX_SAMPLES; ++i) {

		Sample s(itr);

		check->set_current(s);

		check_sample(i);

		s.shift_timestamp(time_start);

		*out << s;

	}

	*out << flush;
}

bool SDCardImpl::process_block(const char* block) {

	bool finished = false;

	BlockIterator itr(block);

	check->set_current_header(itr, block_offset);

	if (check->finished()) {

		finished = true;
	}
	else if (check->datalength()) {

		check->mote_id();

		write_samples(itr);
	}

	return finished;
}
