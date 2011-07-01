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

#include <algorithm>
#include <fstream>
// FIXME Remove when finished
#include <iostream>
#include <stdexcept>
#include "RecordScout.hpp"
#include "RecordID.hpp"
#include "MoteInfo.hpp"
#include "MoteRegistrar.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

template <typename T>
const T& find(const RecordID& rid, const vector<T>& vec) {

	const T rid_to_find = T(rid);

	typename vector<T>::const_iterator pos = lower_bound(vec.begin(), vec.end(), rid_to_find);

	if (pos==vec.end() || !id_equals(*pos, rid_to_find)) {

		string msg("record ID not found ");
		msg.append(rid.str());
		throw logic_error(msg);
	}

	return *pos;
}

void RecordScout::clear() {

	db.clear();

	header.clear();

	records.clear();

	mote_id = card_size_in_blocks = -1;
}

void RecordScout::read_all_existing() {

	clear();

	motes_online.read_all();

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

void RecordScout::push_back_record_info() {

	const int n = static_cast<int> (records.size());

	for (int i=0; i<n; ++i) {

		const Line& line = records.at(i);

		int first_block = line.start_at_block();

		string date = motes_online.date(VirtualMoteID(mote_id, first_block));

		db.push_back(RecordInfo(mote_id, line, date));
	}
}

void RecordScout::push_back() {

	if (records.size() > 0) {

		push_back_record_info();

		typedef vector<Line>::const_reverse_iterator itr;

		itr last_record = records.rbegin();

		const string& date = last_record->download_date();

		int end = last_record->finished_at_block();

		const int n = static_cast<int> (records.size());

		header.push_back(MoteInfo(mote_id, card_size_in_blocks, end, date, n));
	}
}

const vector<MoteInfo>& RecordScout::headers() const {

	return header;
}

const std::vector<RecordInfo>& RecordScout::record_info() const {

	return db;
}

void RecordScout::dump_all() const {

	const int n = static_cast<int> (header.size());

	int position = 0;

	for (int i=0; i<n; ++i) {

		const MoteInfo& moteinfo = header.at(i);

		dump_header(moteinfo);

		const int number_of_records = moteinfo.number_of_records();

		dump_mote(position, number_of_records);

		position += number_of_records;
	}
}

void RecordScout::dump_header(const MoteInfo& moteinfo) const {

	cout << "=========================================================";
	cout << "=========================================================" << endl;
	cout << moteinfo << endl;
}

void RecordScout::dump_mote(const int pos, const int n) const {

	for (int i=0; i<n; ++i) {

		cout << db.at(pos+i) << endl;
	}
}

const MoteInfo& RecordScout::find_moteinfo(const RecordID& rid) const {

	return find(rid, header);
}

const RecordInfo& RecordScout::find_recordinfo(const RecordID& rid) const {

	return find(rid, db);
}

}
