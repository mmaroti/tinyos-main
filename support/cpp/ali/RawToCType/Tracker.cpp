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

#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "Tracker.hpp"
#include "Constants.hpp"

using namespace std;

void Tracker::set_filename(int mote_ID) {

	ostringstream os;

	os << 'm' << setfill('0') << setw(3) << mote_ID << ".rdb" << flush;

	filename = os.str();
}

void Tracker::process_last_line(const string& line) {

	istringstream is(line);

	is.exceptions(istringstream::failbit | istringstream::badbit);

	int begin, end, reboot;

	is >> begin;

	is >> end;

	is >> reboot;

	first_block = end+1;

	reboot_id = reboot;
}

void Tracker::find_last_line(ifstream& in) {

	string line;

	while (in.good()) {

		string buffer;

		getline(in, buffer);

		if (buffer.length()) {
			line = buffer;
		}
	}

	if (line.length()!=0) {

		process_last_line(line);
	}
}

void Tracker::set_first_block_reboot_id() {

	first_block = 0;

	reboot_id = 0;

	ifstream in;

	in.open(filename.c_str());

	find_last_line(in);
}

Tracker::Tracker(int mote_ID) : db(new ofstream()) {

	set_filename(mote_ID);

	set_first_block_reboot_id();

	db->exceptions(ofstream::failbit | ofstream::badbit);

	db->open(filename.c_str(), ofstream::app);
}

Tracker::~Tracker() {

	delete db;
}

int Tracker::start_from_here() const {

	return first_block;
}

int Tracker::reboot() const {

	return reboot_id;
}

const string Tracker::ticks2time(unsigned int t) const {

	ostringstream os;

	unsigned int hour, min, sec, milli;

	hour = t/(3600*TICKS_PER_SEC);
	t =    t%(3600*TICKS_PER_SEC);

	min = t/(60*TICKS_PER_SEC);
	t   = t%(60*TICKS_PER_SEC);

	sec = t/TICKS_PER_SEC;
	t   = t%TICKS_PER_SEC;

	milli = t/(TICKS_PER_SEC/1000.0);

	os << setfill('0') << setw(2) << hour << ":";
	os << setfill('0') << setw(2) << min  << ":";
	os << setfill('0') << setw(2) << sec  << ".";
	os << setfill('0') << setw(3) << milli<< flush;

	return os.str();
}

const string Tracker::current_time() const {

	time_t t;
	time(&t);
	return string(ctime(&t));
}

void Tracker::append(int beg, int end, unsigned int len, int reboot) {

	*db << setw(7) << right << beg    << '\t';
	*db << setw(7) << right << end    << '\t';
	*db << setw(3) << right << reboot << '\t';
	*db << ticks2time(len) << '\t';
	*db << current_time() << flush;

	first_block = end + 1;
	reboot_id = reboot + 1;
}
