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

using namespace std;

const int SECTOR_SIZE = 512;

const int HEADER_LENGTH = 6;
const int SAMPLE_LENGTH = 22;
const int MAX_SAMPLES   = (SECTOR_SIZE-HEADER_LENGTH)/SAMPLE_LENGTH;
const char SEPARATOR    = ',';

typedef unsigned short uint16;
typedef unsigned int uint32;

const uint32 SAMPLING_RATE = 160; // FIXME Also in CompileTimeConstants.hpp
const uint32 TICKS_PER_SEC = 32768;
const int TOLERANCE = 4;

const int UINT16MAX = 0xFFFF;

class sector_iterator {

public:

	explicit sector_iterator(const char* sector) : itr(sector) { }

	uint16 next_uint16() {
		uint16 x = *reinterpret_cast<const uint16*> (itr);
		itr += 2;
		return x;
	}

	uint32 next_uint32() {
		uint32 x = *reinterpret_cast<const uint32*> (itr);
		itr += 4;
		return x;
	}

private:

	const char* itr;

};

class header {

public:

	explicit header(sector_iterator& itr);

	uint16 sector_length() const { return length; }

	uint16 mote() const { return mote_id; }

	friend ostream& operator<<(ostream& , const header& );

private:

	uint16 format_id;
	uint16 mote_id;
	uint16 length;
};

header::header(sector_iterator& itr) {

	format_id = itr.next_uint16();
	mote_id   = itr.next_uint16();
	length    = itr.next_uint16();
}

ostream& operator<<(ostream& out, const header& h) {

	out << "format id: " << h.format_id << endl;
	out << "mote id:   " << h.mote_id   << endl;
	out << "length:    " << h.length    << endl;

	return out;
}

class sample {

public:

	explicit sample(sector_iterator& itr);

	void shift_timestamp(uint32 time_start) { time_stamp -= time_start; }

	bool check_reboot(uint16 counter_previous) const {
		return seq_num==1 && counter_previous!=0;
	}

	int missing(uint16 counter_previous) const {
		int diff = seq_num-counter_previous;
		if (diff < 0) diff += (UINT16MAX+1);
		return diff-1;
	}

	int error_in_ticks(uint32 time_previous) const {
		uint32 expected = time_previous + SAMPLING_RATE;
		return time_stamp - expected;
	}

	uint32 timestamp() const { return time_stamp; }

	uint16 counter() const { return seq_num; }

	friend ostream& operator<<(ostream& , const sample& );

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

sample::sample(sector_iterator& itr) {

	time_stamp = itr.next_uint32();
	seq_num    = itr.next_uint16();
	acc_x      = itr.next_uint16();
	acc_y      = itr.next_uint16();
	acc_z      = itr.next_uint16();
	gyro_x     = itr.next_uint16();
	gyro_y     = itr.next_uint16();
	gyro_z     = itr.next_uint16();
	volt       = itr.next_uint16();
	temp       = itr.next_uint16();
}

ostream& operator<<(ostream& out, const sample& s) {

	out << s.time_stamp << SEPARATOR;
	out << s.seq_num    << SEPARATOR;
	out << s.acc_x      << SEPARATOR;
	out << s.acc_y      << SEPARATOR;
	out << s.acc_z      << SEPARATOR;
	out << s.gyro_x     << SEPARATOR;
	out << s.gyro_y     << SEPARATOR;
	out << s.gyro_z     << SEPARATOR;
	out << s.volt       << SEPARATOR;
	out << s.temp       << '\n';

	return out;
}

ifstream in;

ofstream out;

char buffer[SECTOR_SIZE];

const char* read_sector(int i) {

	const char* ret_val = 0;

	try {

		in.read(buffer, SECTOR_SIZE);

		ret_val = buffer;
	}
	catch (...) {

	}

	return ret_val;
}

void init(const char* infile) {

	in.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

	in.open(infile, ios::binary);

	out.exceptions(ofstream::failbit | ofstream::badbit);
}

uint32 time_start = 0;

uint32 time_previous = 0;

uint16 counter_previous = 1;

uint32 samples_processed = 0;

uint16 mote_id = 0; // TODO Mote ID is not checked.

int sector_offset = 0;

int reboot_seq_num = 0;

void create_new_file() {

	ostringstream os;

	os << 'm' << setfill('0') << setw(2) << mote_id << '_';
	os << 'r' << setfill('0') << setw(3) << reboot_seq_num << '_';
	os << 's' << sector_offset << ".csv" << flush;

	if (out.is_open())
		out.close();

	out.open(os.str().c_str());
}

bool reboot(const sample& s, int i) {

	bool reboot = s.check_reboot(counter_previous);

	if (reboot && i==0) {

		cout << "Found a reboot at sample " << samples_processed << endl;

		++reboot_seq_num;

		time_start = s.timestamp();

		create_new_file();

		return true;
	}

	return false;
}

void check_counter(const sample& s) {

	int missed = s.missing(counter_previous);

	if (missed) {
		clog << "Warning: at sample " << samples_processed;
		clog << " missing at least ";
		clog << missed << " samples" << endl;
	}
}

void check_timestamp(const sample& s) {

	int dt = s.error_in_ticks(time_previous);

	if (abs(dt)>TOLERANCE) {
		clog << "Warning: at sample " << samples_processed << " ";
		clog << dt << " ticks error" << endl;
	}
}

void check_sample(const sample& s, const int i) {

	if (!reboot(s, i)) {

		check_counter(s);

		check_timestamp(s);
	}

	time_previous = s.timestamp();

	counter_previous = s.counter();
}

void write_samples(sector_iterator& itr) {

	for (int i=0; i<MAX_SAMPLES; ++i) {

		sample s(itr);

		check_sample(s, i);

		s.shift_timestamp(time_start);

		out << s;

		++samples_processed;
	}

	out << flush;
}

bool process_sector(const char* sector) {

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

int main() {

	init("oct28_2");

	bool finished = false;

	const char* sector = 0;

	while ((sector = read_sector(sector_offset++)) && !finished ) {

		finished = process_sector(sector);
	}

	return 0;
}
