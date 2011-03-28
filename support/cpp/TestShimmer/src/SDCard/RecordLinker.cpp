/* Copyright (c) 2010, 2011 University of Szeged
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
*      Author: Ali Baharev
*/

#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include "Constants.hpp"
#include "FlatFileDB.hpp"
#include "RecordLinker.hpp"
#include "RecordInfo.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

RecordLinker::RecordLinker(const char* filename) : out(new ofstream(filename)) {

	if (!out->is_open()) {

		throw runtime_error("failed to create output file");
	}

	out->exceptions(ios_base::failbit | ios_base::badbit);

	*out << "# Mote and reboot IDs involved\n";

	skew_1 = offset = 0;

	global_start = numeric_limits<unsigned int>::max();

	mote = reboot = -1;
}

void RecordLinker::write_participant(int mote, int record) {

	*out << mote << '/' << record << '\t';
}

void RecordLinker::write_reference_boot_time(unsigned int global_start_utc) {

	global_start = global_start_utc;

	*out << "\n# Mote providing reference time booted at (Unix time, zero if unknown)\n";
	*out << global_start << '\n';

}

void RecordLinker::write_record(int Mote,
		                        int Reboot,
		  			            const std::string& Length,
		                        unsigned int Boot_utc,
		                        double Skew_1,
		                        double Offset)
{
	mote     = Mote;
	reboot   = Reboot;
	length   = Length;
	boot_utc = Boot_utc;
	skew_1   = Skew_1;
	offset   = Offset;

	write_header();

	write_data();
}

void RecordLinker::write_header() {

	*out << "# Mote and reboot ID\n";
	*out << mote << '/' << reboot << '\n';
	*out << "# Length of record\n";
	*out << length << '\n';
	*out << "# Unix time of boot (zero if unknown)\n";
	*out << boot_utc << '\n';
	*out << "# Skew-1 and offset (second)\n";
	*out << skew_1/1024 << '/' << offset/1024 << '\n';
	*out << "# Unix time, mote time (32kHz ticks), counter, ";
	*out << "accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, volt, temp\n";
}

void RecordLinker::write_data() {

	ifstream in(record_file_name().c_str());

	if (!in.is_open()) {

		throw runtime_error("failed to open "+record_file_name());
	}

	while (in.good()) {

		string buffer;

		getline(in, buffer);

		write_line(buffer);
	}
}

void RecordLinker::write_line(const std::string& line) {

	if (line.empty()) {

		return;
	}

	istringstream iss(line);

	iss.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);

	unsigned int mote_time;

	iss >> mote_time;

	*out << mote_time2global_time(mote_time) << ',' << line << '\n';
}

const string RecordLinker::mote_time2global_time(unsigned int mote_time) const {

	double time = (mote_time/((double)TICKS_PER_SEC))*1024; // Binary ms

	const double skew_corr = skew_1*time;

	time += skew_corr;

	time += offset;

	time /= 1024; // second

	time += global_start;

	ostringstream oss;

	oss << fixed << setprecision(3) << time << flush;

	return oss.str();
}

const string RecordLinker::record_file_name() const {

	FlatFileDB db(mote);

	int start_at = db.first_block(reboot);

	return get_filename(mote, reboot, start_at)+".csv";
}

RecordLinker::~RecordLinker() {

	*out << '\n' << flush;
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
