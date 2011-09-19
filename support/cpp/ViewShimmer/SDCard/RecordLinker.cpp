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

#include <cstdlib>
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

#include <QtDebug>

using namespace std;

namespace sdc {

RecordLinker::RecordLinker(const char* filename) : out(new ofstream(filename)) {

	if (!out->is_open()) {

		throw runtime_error("failed to create output file");
	}

	out->exceptions(ios_base::failbit | ios_base::badbit);

	*out << "#mote,reboot_ID,length,boot_unix_time,skew_1,offset\n";

	skew_1 = offset = 0;

	global_start = numeric_limits<unsigned int>::max();

	mote = reboot = -1;
}

void RecordLinker::write_participant(
		int Mote,
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
}

void RecordLinker::write_header() {

	*out << mote << ',' << reboot << ',' << length_to_second() << ',';
	*out << boot_utc << ',' << skew_1/1024 << ',' << offset/1024 << '\n';
}

void RecordLinker::set_reference_boot_time(unsigned int global_start_utc) {

	global_start = global_start_utc;
}

void RecordLinker::write_record_header() {

	*out << '\n';
	*out << "#mote,reboot_ID,unix_time,mote_time,counter,";
	*out << "accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp\n";
}

void RecordLinker::write_record(int Mote,
		                        int Reboot,
		                        double Skew_1,
		                        double Offset)
{
	mote     = Mote;
	reboot   = Reboot;
	skew_1   = Skew_1;
	offset   = Offset;

	write_data();
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

	*out << mote << ',' << reboot << ',' << mote_time2global_time(mote_time) << ',';
	*out << line << '\n';
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

// FIXME In the new SQLite DB, store length in ms or ticks, not as string
const string RecordLinker::length_to_second() const {

	// 01 2 34 5 67 8 901
	// hh : mm : ss . mmm
	if (!(length.at(2)==':' &&
		  length.at(5)==':' &&
		  length.at(8)=='.' &&
		  length.size()==12))
	{
		throw runtime_error("invalid time format: "+length);
	}

	int time = 0;

	time += atoi(length.substr(0, 2).c_str())*3600;
	time += atoi(length.substr(3, 2).c_str())*60;
	time += atoi(length.substr(6, 2).c_str());

	return int2str(time)+length.substr(8);
}

const string RecordLinker::record_file_name() const {

	FlatFileDB db(mote);

	int start_at = db.first_block(reboot);

        QString vmi = QString::fromStdString(get_filename(mote, reboot, start_at));
        qDebug() << vmi;

	return get_filename(mote, reboot, start_at)+".csv";
}

RecordLinker::~RecordLinker() {

	*out << '\n' << flush;
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
