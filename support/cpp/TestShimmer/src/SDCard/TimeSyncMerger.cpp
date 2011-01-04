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
#include <stdexcept>
#include "TimeSyncMerger.hpp"
#include "TimeSyncInfo.hpp"
#include "TimeSyncReader.hpp"
#include "FlatFileDB.hpp"
#include "Merger.hpp"
#include "RecordPairID.hpp"
#include "VirtualMoteID.hpp"

using namespace std;

namespace sdc {

TimeSyncMerger::TimeSyncMerger(int mote, int reboot)
: db1(new FlatFileDB(mote)),
  mote1(mote),
  reboot1(reboot),
  block1(db1->first_block(reboot)),
  rec1(RecordID(mote, reboot))
{
	mote2   = -1;
	reboot2 = -1;
	block2  = -1;

	VirtualMoteID vmote1(mote1, block1);

	TimeSyncReader reader1(mote1, reboot1, block1);

	int length1 = db1->length_in_ms(reboot1);

	merger.reset( new Merger(vmote1, reader1.messages_as_list(), length1) );

	process_pairs();

	check_size();

}

void TimeSyncMerger::check_size() const {

	if (result.size()!=otherRecords.size()) {

		throw logic_error("internal error in TimeSyncMerger");
	}
}

void TimeSyncMerger::reset_db_if_needed() {

	if (merger->mote2_id_changed()) {

		mote2 = merger->mote2_id();

		db2.reset(new FlatFileDB(mote2));
	}
}

void TimeSyncMerger::process_pairs() {

	while(merger->set_next()) {

		reset_db_if_needed();

		block2 = merger->block2();

		reboot2 = db2->reboot(block2);

		TimeSyncReader reader2(mote2, reboot2, block2);

		int length2 = db2->length_in_ms(reboot2);

		merger->set_mote2_messages(reader2.messages_as_list(), length2);

		merge();
	}

	merger.reset();
}

void TimeSyncMerger::merge() {

	const int size = merger->merge();

	if (size<3) {

		return;
	}

	const RecordID rec2(mote2, reboot2);

	insert(rec2);

	RecordPairID id(rec1, rec2);

	insert(id, merger->results_in_mote_id_order());

	check_size();
}

void TimeSyncMerger::insert(const RecordID& other) {

	pair<Set::iterator, bool> pos = otherRecords.insert(other);

	if (!pos.second) {

		throw logic_error("this record has already been added");
	}
}

void TimeSyncMerger::insert(const RecordPairID& id, const vector<Pair>& sync_points) {

	if (sync_points.size()<3) {

		throw logic_error("sync point size should be >= 3");
	}

	typedef std::pair<const RecordPairID, std::vector<Pair> > RecordPair;

	pair<Map::iterator, bool> pos = result.insert(RecordPair(id, sync_points));

	if (!pos.second) {

		throw logic_error("this record pair has already been added");
	}

	cout << "Added " << id << ", size " << sync_points.size() << endl;
}

const Map& TimeSyncMerger::pairs() const {

	return result;
}

const Set& TimeSyncMerger::recordID_of_pairs() const {

	return otherRecords;
}

TimeSyncMerger::~TimeSyncMerger() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
