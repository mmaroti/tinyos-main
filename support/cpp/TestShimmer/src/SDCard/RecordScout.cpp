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
// FIXME Remove when finished
#include <iostream>
#include "RecordScout.hpp"
#include "MoteInfo.hpp"
#include "MoteRegistrar.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

void RecordScout::clear() {

	db.clear();

	header.clear();

	records.clear();

	mote_id = card_size_in_blocks = -1;
}

void RecordScout::read_all_existing() {

	clear();

	mote_date_online.read_all();

	const vector<MoteID_Size> ids = MoteRegistrar::existing_ids();

	const int n = static_cast<int> (ids.size());

	for (int i=0; i<n; ++i) {

		const MoteID_Size m = ids.at(i);

		mote_id = m.mote_id();

		card_size_in_blocks = m.size_in_blocks();

		records.clear();

		read_mote_rdb();

		push_back();
	}
}

void RecordScout::read_mote_rdb() {

	ifstream in;

	in.open(rdb_file_name(mote_id).c_str());

	while (in.good()) {

		string buffer;

		getline(in, buffer);

		push_line(buffer);
	}
}

void RecordScout::push_line(const string& buffer) {

	if (buffer.size()!=0) {

		records.push_back(Line(buffer));
	}
}

void RecordScout::push_back() {

	if (records.size() > 0) {

		db.push_back(pair<int, vector<Line> > (mote_id, records));

		typedef vector<Line>::const_reverse_iterator itr;

		itr last_record = records.rbegin();

		const string& date = last_record->download_date();

		int end = last_record->finished_at_block();

		header.push_back(MoteInfo(mote_id, card_size_in_blocks, end, date));
	}
}

void RecordScout::dump_all() const {

	const int n = static_cast<int> (db.size());

	for (int i=0; i<n; ++i) {

		dump_header(header.at(i));

		const pair<int, vector<Line> >& p = db.at(i);

		dump_mote(p.first, p.second);
	}
}

void RecordScout::dump_header(const MoteInfo& moteinfo) const {

	cout << "=========================================================";
	cout << "=========================================================" << endl;
	cout << moteinfo << endl;
}

void RecordScout::dump_mote(int mote, const vector<Line>& record) const {

	const int n = static_cast<int> (record.size());

	for (int i=0; i<n; ++i) {

		const Line& line = record.at(i);
		int first_block = line.start_at_block();

		cout << mote << '\t' << line << '\t';
		cout << mote_date_online.date(VirtualMoteID(mote, first_block)) << endl;
	}
}

}
