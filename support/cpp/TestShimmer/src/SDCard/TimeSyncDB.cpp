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

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "TimeSyncDB.hpp"
#include "Constants.hpp"

using namespace std;

typedef istringstream iss;
typedef ostringstream oss;

namespace sdc {

void TimeSyncDB::read_all() {

	motes.clear();

	in.reset(new ifstream(MOTE_DATE_DB));

	if (in->good()) {

		grab_line();
	}

	in.reset();
}

void TimeSyncDB::grab_line() {

	while (in->good()) {

		line.clear();

		getline(*in, line);

		if (line.size()==0) {

			break;
		}

		parse_line();

		push();
	}
}

void TimeSyncDB::parse_line() {

	try {

		parse();
	}
	catch(ios_base::failure& ) {

		oss os;

		os << "failed to parse \"" << line << "\" in " << MOTE_DATE_DB << flush;

		throw runtime_error(os.str());
	}
}

void TimeSyncDB::parse() {

	iss ss(line);

	ss.exceptions(iss::failbit | iss::badbit);

	ss >> vmote_id;

	unsigned int dummy_mote_time;

	ss >> dummy_mote_time;

	ss.get();

	getline(ss, date_seen);
}

void TimeSyncDB::push() {

	Pair result = motes.insert(make_pair(vmote_id, date_seen));

	if (result.second == false) {

		const string& date = result.first->second;

		cout << "Warning: duplicate " << vmote_id << ", only ";
		cout << date << " is kept" << endl;
	}
}

const string TimeSyncDB::date(const VirtualMoteID& vmote) const {

	Map::const_iterator i = motes.find(vmote);

	return (i!=motes.end()) ? i->second : string();
}

TimeSyncDB::~TimeSyncDB() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
