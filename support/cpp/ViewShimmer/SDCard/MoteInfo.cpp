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

#include <ostream>
#include "MoteInfo.hpp"
#include "RecordID.hpp"
#include "BlockRelatedConsts.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

MoteInfo::MoteInfo() {

	mote_ID = -1;
}

MoteInfo::MoteInfo( int mote,
		double size,
		int last_block,
		const string& date,
		int number_of_records)
{
	mote_ID = mote;

	double card_size_GB = (size*BLOCK_SIZE)/GB();

	hours_remaining = sdc::remaining_hours(card_size_GB, last_block);

	last_seen = date;

	num_of_records = number_of_records;
}

MoteInfo::MoteInfo(const RecordID& rid) {

	mote_ID = rid.mote_ID;
}

int MoteInfo::mote_id() const {

	return mote_ID;
}

const string& MoteInfo::last_download() const {

	return last_seen;
}

const string& MoteInfo::remaining_hours() const {

	return hours_remaining;
}

int MoteInfo::number_of_records() const {

	return num_of_records;
}

std::ostream& operator<<(std::ostream& out, const MoteInfo& m) {

	out << "Mote id " << m.mote_id() << ", " << m.remaining_hours();
	out << " hours remaining, last download on " << m.last_download() << ", ";
	out << m.number_of_records() << " records in the database";

	return out;
}

bool operator<(const MoteInfo& lhs, const MoteInfo& rhs) {

	return lhs.mote_id()<rhs.mote_id();
}

bool id_equals(const MoteInfo& info1, const MoteInfo& info2) {

	return info1.mote_id()==info2.mote_id();
}

}
