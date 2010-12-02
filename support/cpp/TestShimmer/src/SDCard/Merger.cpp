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
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include "Merger.hpp"
#include "TimeSyncInfo.hpp"
#include "TimeSyncConsts.hpp"

using namespace std;

namespace sdc {

typedef List::iterator li;
typedef List::const_iterator cli;
typedef List::size_type Size_t;

Merger::Merger(const VirtualMoteID& vmote, const List& msg, int length_in_ms)
	: vmote1(vmote), length1(length_in_ms), mote1(msg)
{
	drop_inconsistent(mote1);

	mote2_id_new = false;

	cout << "Initialized mote1 list" << endl;
}

void Merger::drop_inconsistent(List& messages) {

	cout << "Dropping inconsistent messages" << endl;

	Size_t size_before = messages.size();

	for (li i = messages.begin(); i != messages.end(); ) {

		if (!i->consistent()) {

			cout << "Warning: inconsistent message " << endl << *i << endl;

			i = messages.erase(i);
		}
		else {

			++i;
		}
	}

	cout << "Dropped " << size_before - messages.size() << " messages" << endl;
}

void Merger::log_msg_loss(const List& messages, const VirtualMoteID& vmid) const {

	if (messages.empty()) {

		return;
	}

	cout << "Checking message frequency on: " << vmid << endl;

	cli i = messages.begin();

	TimeSyncInfo previous = *i;

	++i;

	while (i != messages.end()) {

		const TimeSyncInfo current = *i;

		int lost = current.lost_messages_since(previous);

		if (lost) {

			cout << "Warning: missing " << lost << " messages" << endl;
		}

		previous = current;

		++i;
	}

	cout << "Message loss checked" << endl;
}

void Merger::init_for_mote2() {

	mote2.clear();
	temp.clear();
	merged.clear();

	VirtualMoteID vmote = *mote1.begin();

	mote2_id_new = vmote.mote_id()==vmote2.mote_id() ? false : true;

	vmote2 = vmote;

	cout << "-----------------------------------------------------" << endl;
	cout << "Found: " << vmote2 << endl;
}

bool Merger::set_next() {

	if (mote1.empty()) {

		return false;
	}

	init_for_mote2();

	for (li i = mote1.begin(); i != mote1.end(); ) {

		if (vmote2 == *i) {

			temp.push_back(*i);

			i = mote1.erase(i);
		}
		else {

			++i;
		}
	}

	cout << temp.size() << " relevant messages from " << vmote1 << " copied";
	cout << endl;

	log_msg_loss(temp, vmote1);

	return !temp.empty(); // FIXME temp.empty()==false
}

bool Merger::mote2_id_changed() const {

	return mote2_id_new;
}

int Merger::mote2_id() const {

	return vmote2.mote_id();
}


int Merger::block2() const {

	return vmote2.first_block();
}

void Merger::set_mote2_messages(const List& messages_mote2, int length2_in_ms) {

	mote2 = messages_mote2;

	length2 = length2_in_ms;

	drop_inconsistent(mote2);

	drop_not_from_mote1();

	log_msg_loss(mote2, vmote2);
}

void Merger::drop_not_from_mote1() {

	cout << "Discarding messages not from " << vmote1 << endl;

	Size_t size_before = mote2.size();

	for (li i = mote2.begin(); i != mote2.end(); ) {

		if (vmote1 != *i) {

			i = mote2.erase(i);
		}
		else {

			++i;
		}
	}

	cout << mote2.size() << " messages remain, dropped ";
	cout << size_before - mote2.size() << endl;
}

void Merger::handle_conflicting_keys(mi& pos, const Pair& sync_point) {

	uint32 point1 = sync_point.second;
	uint32 point2 = pos->second;

	cout << "Warning: conflicting keys in time pairs " << endl;
	cout << sync_point.first << '\t' << point1 << endl;
	cout << pos->first       << '\t' << point2 << endl;

	if (point1>point2) {
		swap(point1, point2);
	}

	uint32 diff = point2-point1;

	if (diff > static_cast<uint32>(OFFSET_TOLERANCE)) {
		cout << "they differ too much, dropping both" << endl;
		merged.erase(pos);
	}
	else {
		pos->second = point1 + (diff >> 1);
		cout << "merged them as" << endl;
		cout << pos->first << '\t' << pos->second << endl;
	}
}

void Merger::insert(const Pair& sync_point) {

	pair<Map::iterator, bool> result = merged.insert(sync_point);

	if (!result.second) {

		handle_conflicting_keys(result.first, sync_point);
	}
}

int Merger::offset(const CPair& p) const {

	return p.first - p.second;
}

int Merger::initial_offset() const {

	const int n = 3, middle = 1;

	int ofs[n];

	int i=0;

	for (cmi j = merged.begin(); i<n; ++i, ++j) {

		ofs[i] = offset(*j);
	}

	cout << "1st, 2nd, 3rd offset: ";
	cout << ofs[0] << "  " << ofs[1] << "  " << ofs[2] << endl;

	sort(ofs, ofs+n);

	return ofs[middle];
}

bool Merger::wrong_offset(const CPair& time_pair, int& previous_offset) const {

	bool wrong = false;

	int current_offset = offset(time_pair);

	int diff = current_offset-previous_offset;

	if (abs(diff) > OFFSET_TOLERANCE) {

		cout << "Warning: wrong offset found, previous " << previous_offset;
		cout << ", current " << current_offset << ", diff " << diff << endl;

		wrong = true;
	}
	else {

		previous_offset = current_offset;
	}

	return wrong;
}

bool Merger::sufficient_size() {

	bool sufficient = true;

	if (merged.size() < 3) {

		cout << "Warning: time sync requires at least 3 sync points" << endl;

		merged.clear();

		sufficient = false;
	}

	return sufficient;
}

void Merger::drop_wrong_offsets() {

	if (!sufficient_size()) {

		return;
	}

	int offset = initial_offset();

	int k = 1;

	for (mi i = merged.begin(); i!=merged.end(); ++k) {

		if (wrong_offset(*i, offset)) {

			cout << "Erasing wrong time pair " << i->first << "  ";
			cout << i->second << " (" << k << ")" << endl;

			merged.erase(i++);
		}
		else {
			++i;
		}
	}

	sufficient_size();
}

void Merger::log_size_before_merge() const {

	cout << "Merging time sync info" << endl;
	cout << "Number of messages " << temp.size() << " + " << mote2.size();
	cout << " = " << temp.size() + mote2.size() << endl;

	int diff = temp.size()-mote2.size();

	if ( abs(diff) > 2 ) {

		cout << "Warning: difference = " << abs(diff) << " > 2" << endl;
	}

	double length = min(length1, length2);

	int estimated_msg = floor(length/TIMESYNC_MSG_RATE+0.5);

	cout << "Estimated: " << estimated_msg << " / mote" << endl;

	unsigned int max_expected = estimated_msg + 1;

	if (temp.size() > max_expected || mote2.size() > max_expected) {

		cout << "Warning: more messages than expected" << endl;
	}
}

int Merger::merge() {

	log_size_before_merge();

	if (!merged.empty()) {

		throw logic_error("merged should have been cleared");
	}

	for (cli i = temp.begin(); i != temp.end(); ++i) {

		insert(i->time_pair());
	}

	for (cli i = mote2.begin(); i != mote2.end(); ++i) {

		insert(i->reversed_time_pair());
	}

	drop_wrong_offsets();

	cout << "Merged, size: " << merged.size() << endl;

	return static_cast<int>(merged.size());
}

void Merger::copy_in_reveresed_order(std::vector<Pair>& pairs) const {

	for (cmi i=merged.begin(); i!=merged.end(); ++i) {

		pairs.push_back( Pair(i->second, i->first) );
	}
}

const std::vector<Pair> Merger::results_in_mote_id_order() const {

	vector<Pair> pairs;

	pairs.reserve(merged.size());

	if (vmote1.mote_id() < vmote2.mote_id()) {

		pairs.assign( merged.begin(), merged.end() );
	}
	else if (vmote1.mote_id() > vmote2.mote_id()) {

		copy_in_reveresed_order(pairs);
	}
	else {

		throw logic_error("mote cannot be in pair with itself");
	}

	return pairs;
}

}
