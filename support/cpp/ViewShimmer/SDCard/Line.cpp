/* Copyright (c) 2010, University of Szeged
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

#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include "Line.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

typedef istringstream iss;

Line::Line(const string& line) {

	iss in(line);

	in.exceptions(iss::failbit | iss::badbit);

	in >> first_block;
	in >> last_block;
	in >> reboot;

	if (first_block < 0 || first_block > last_block || reboot < 1) {
		throw runtime_error("corrupted line");
	}

	in >> time_length;
	in >> computed_length;

	in.get();

	getline(in, date);
}

Line::Line(int first, int last, int reboot_id, unsigned int time_len)
	: date(current_time())
{
	first_block = first;
	last_block  = last;
	reboot      = reboot_id;
	time_length = ticks2time(time_len);
	computed_length = recorded_length(first_block, last_block);
}

void Line::consistent_with(const Line& previous) const {

	if ((first_block != previous.last_block+1) ||
		(reboot      != previous.reboot   +1) )
	{
		throw runtime_error("corrupted database");
	}
}

int Line::start_at_block() const {

	return first_block;
}

int Line::finished_at_block() const {

	return last_block;
}

int Line::reboot_id() const {

	return reboot;
}

const string& Line::record_length() const {

	return computed_length;
}

const string& Line::download_date() const {

	return date;
}

ostream& operator<<(ostream& out, const Line& line) {

	out << setw(7) << right << line.first_block << '\t';
	out << setw(7) << right << line.last_block  << '\t';
	out << setw(3) << right << line.reboot      << '\t';
	out << line.time_length     << '\t';
	out << line.computed_length << '\t';
	out << line.date;

	return out;
}

}
