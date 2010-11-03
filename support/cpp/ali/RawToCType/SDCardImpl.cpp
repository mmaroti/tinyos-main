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
#include "RawDevice.hpp"

using namespace std;

const int HEADER_LENGTH = 6;
const int SAMPLE_LENGTH = 22;
const int MAX_SAMPLES   = (SECTOR_SIZE-HEADER_LENGTH)/SAMPLE_LENGTH;
const int TOLERANCE = 4;

SDCardImpl::SDCardImpl(RawDevice* source)
	: device(source), out(new ofstream())
{
	out->exceptions(ofstream::failbit | ofstream::badbit);
	time_start = 0;
	time_previous = 0;
	counter_previous = 1;
	samples_processed = 0;
	mote_id = 0; // TODO Mote ID is not checked.
	sector_offset = 0;
	reboot_seq_num = 0;
}

void SDCardImpl::process_new_measurements() {

	bool finished = false;

	const char* sector = 0;

	while ((sector = device->read_sector(sector_offset)) && !finished ) {

		finished = process_sector(sector);

		++sector_offset;
	}
}

SDCardImpl::~SDCardImpl() {

	delete device;
	delete out;
}

void SDCardImpl::create_new_file() {

	ostringstream os;

	os << 'm' << setfill('0') << setw(2) << mote_id << '_';
	os << 'r' << setfill('0') << setw(3) << reboot_seq_num << '_';
	os << 's' << sector_offset << ".csv" << flush;

	if (out->is_open())
		out->close();

	out->open(os.str().c_str());
}

bool SDCardImpl::reboot(const int sample_in_sector) {

	bool reboot = s.check_reboot(counter_previous);

	if (reboot && sample_in_sector==0) {

		cout << "Found a reboot at sample " << samples_processed << endl;

		++reboot_seq_num;

		time_start = s.timestamp();

		create_new_file();

		return true;
	}

	return false;
}

void SDCardImpl::check_counter() {

	int missed = s.missing(counter_previous);

	if (missed) {
		clog << "Warning: at sample " << samples_processed;
		clog << " missing at least ";
		clog << missed << " samples" << endl;
	}
}

void SDCardImpl::check_timestamp() {

	int dt = s.error_in_ticks(time_previous);

	if (abs(dt)>TOLERANCE) {
		clog << "Warning: at sample " << samples_processed << " ";
		clog << dt << " ticks error" << endl;
	}
}

void SDCardImpl::check_sample(const int sample_in_sector) {

	if (!reboot(sample_in_sector)) {

		check_counter();

		check_timestamp();
	}

	time_previous = s.timestamp();

	counter_previous = s.counter();
}

void SDCardImpl::write_samples(sector_iterator& itr) {

	for (int i=0; i<MAX_SAMPLES; ++i) {

		s = sample(itr);

		check_sample(i);

		s.shift_timestamp(time_start);

		*out << s;

		++samples_processed;
	}

	*out << flush;
}

bool SDCardImpl::process_sector(const char* sector) {

	sector_iterator itr(sector);

	const header h(itr);

	const int length = h.sector_length();

	if (length == 0) {
		cout << "Finished!" << endl;
		return true;
	}

	if (MAX_SAMPLES != length/SAMPLE_LENGTH) {

		clog << "Warning: invalid length in sector " << sector_offset << endl;
	}
	else {
		mote_id = h.mote();
		write_samples(itr);
	}

	return false;
}
