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
#include "RecordInfo.hpp"
#include "RecordID.hpp"
#include "Line.hpp"

using namespace std;

namespace sdc {


RecordInfo::RecordInfo(int mote_id, const Line& line, const string& date_rec) {

	mote   = mote_id;
	record = line.reboot_id();
	len    = line.record_length();
	date_of_download = line.download_date();
	date_of_record   = date_rec;
}

RecordInfo::RecordInfo(const RecordID &rid) {

	mote = rid.mote_ID;
	record = rid.reboot_ID;
}

int RecordInfo::mote_id() const {

	return mote;
}

int RecordInfo::record_id() const {

	return record;
}

const string& RecordInfo::length() const {

	return len;
}
const string& RecordInfo::date_downloaded() const {

	return date_of_download;
}
const string& RecordInfo::date_recorded() const {

	return date_of_record;
}

std::ostream& operator<<(std::ostream& out, const RecordInfo& r) {

	out << r.mote_id() << '\t' << r.record_id() << '\t' << r.length() << '\t';
	out << r.date_downloaded() << '\t' << r.date_recorded();

	return out;
}

bool operator<(const RecordInfo& info1, const RecordInfo& info2) {

	return (info1.mote_id()!=info2.mote_id()) ?
			info1.mote_id()<info2.mote_id() :
			info1.record_id()<info2.record_id();
}

bool id_equals(const RecordInfo& info1, const RecordInfo& info2) {

	return info1.mote_id()==info2.mote_id() && info1.record_id()==info2.record_id();
}

}
