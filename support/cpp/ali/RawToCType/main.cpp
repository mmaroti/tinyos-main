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
#include <assert.h>

using namespace std;

const int SECTOR_SIZE = 512;
char buffer[SECTOR_SIZE];

const int HEADER_LENGTH = 6;
const int SAMPLE_LENGTH = 22;
const int MAX_SAMPLES   = (SECTOR_SIZE-HEADER_LENGTH)/SAMPLE_LENGTH;
const char SEPARATOR    = ',';

typedef unsigned short uint16;
typedef unsigned int uint32;

class raw_sector {

public:

	explicit raw_sector(const char* sector) : data(sector) { }

	uint16 next_uint16() {
		uint16 x = *reinterpret_cast<const uint16*> (data);
		data += 2;
		return x;
	}

	uint32 next_uint32() {
		uint32 x = *reinterpret_cast<const uint32*> (data);
		data += 4;
		return x;
	}

private:

	const char* data;

};

class header {

public:

	explicit header(raw_sector& data);

	uint16 sector_length() const { return length; }

	friend ostream& operator<<(ostream& , const header& );

private:

	uint16 format_id;
	uint16 mote_id;
	uint16 length;
};

header::header(raw_sector& data) {

	format_id = data.next_uint16();
	mote_id   = data.next_uint16();
	length    = data.next_uint16();
}

ostream& operator<<(ostream& out, const header& h) {

	out << "format id: " << h.format_id << endl;
	out << "mote id:   " << h.mote_id   << endl;
	out << "length:    " << h.length    << endl;

	return out;
}

class sample {

public:

	explicit sample(raw_sector& data);

	friend ostream& operator<<(ostream& , const sample& );

private:

	uint32 time_stamp;
	uint16 counter;
	uint16 acc_x;
	uint16 acc_y;
	uint16 acc_z;
	uint16 gyro_x;
	uint16 gyro_y;
	uint16 gyro_z;
	uint16 volt;
	uint16 temp;
};

sample::sample(raw_sector& data) {

	time_stamp = data.next_uint32();
	counter    = data.next_uint16();
	acc_x      = data.next_uint16();
	acc_y      = data.next_uint16();
	acc_z      = data.next_uint16();
	gyro_x     = data.next_uint16();
	gyro_y     = data.next_uint16();
	gyro_z     = data.next_uint16();
	volt       = data.next_uint16();
	temp       = data.next_uint16();
}

ostream& operator<<(ostream& out, const sample& s) {

	out << s.time_stamp << SEPARATOR;
	out << s.counter    << SEPARATOR;
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

const char* read_sector(ifstream& in) {

	const char* ret_val = 0;

	try {

		in.read(buffer, SECTOR_SIZE);

		ret_val = buffer;
	}
	catch (...) {

	}

	return ret_val;
}

void init(ifstream& in) {

	in.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

	in.open("oct28_2", ios::binary);
}

int main() {

	ifstream in;

	init(in);

	ofstream out;

	out.exceptions(ofstream::failbit | ofstream::badbit);

	out.open("shimmer_processed.txt");

	while (const char* sec = read_sector(in)) {

		raw_sector data(sec);

		header h(data);

		const int length = h.sector_length();

		if (length == 0) {
			break;
		}

		assert(MAX_SAMPLES == length/SAMPLE_LENGTH);

		for (int i=0; i<MAX_SAMPLES; ++i) {

			sample s(data);
			out << s;
		}

		out << flush;
	}

	return 0;
}
