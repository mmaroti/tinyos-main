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

#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include "FlatFileDB.hpp"
#include "Line.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

FlatFileDB::FlatFileDB(int mote_ID) : mote_id(mote_ID) {

	if (mote_id<=0) {

		throw runtime_error("mote id must be positive");
	}

	string fname = rdb_file_name(mote_id);

	ifstream in;

	in.open(fname.c_str());

	int last_used_block = -1;

	if (in.is_open()) {

		last_used_block = read_file(in);
	}

	if (record.size()==0) {

		throw_not_downloaded_error();
	}

	size = static_cast<int>(record.size());

	record.push_back(last_used_block+1);

	cout << "DB of mote " << mote_ID << " is opened" << endl;
}

void FlatFileDB::throw_not_downloaded_error() const {

	string msg("perhaps records of mote ");
	msg.append(int2str(mote_id));
	msg.append(" have not been downloaded yet");

	throw runtime_error(msg);
}

int FlatFileDB::read_file(std::ifstream& in) {

	Line previous(0, -1, 0, 0);

	while (in.good()) {

		string buffer;

		getline(in, buffer);

		push_back(buffer, previous);
	}

	return previous.finished_at_block();
}

void FlatFileDB::push_back(const string& line, Line& previous) {

	if (line.size()==0) {

		return;
	}

	Line current(line);

	current.consistent_with(previous);

	previous = current;

	record.push_back(current.start_at_block());
}

int FlatFileDB::reboot(int first_block) const {

	if (first_block > record.at(size-1)) {

		throw_not_downloaded_error();
	}

	vector<int>::const_iterator pos = lower_bound(record.begin(), record.end(), first_block);

	int index = pos - record.begin();

	if (first_block != record.at(index)) {

		string msg("failed to find ");
		msg.append(int2str(first_block));
		msg.append(" as first block");

		throw runtime_error(msg);
	}

	return index + 1;
}

int FlatFileDB::first_block(int reboot) const {

	return record.at(reboot-1);
}

int FlatFileDB::number_of_records() const {

	return size;
}

int FlatFileDB::length_in_ms(int reboot) const {

	int first = record.at(reboot-1);
	int last  = record.at(reboot) - 1;

	return recorded_length_in_ms(first, last);
}

}
