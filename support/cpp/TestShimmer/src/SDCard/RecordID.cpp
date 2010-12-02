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
#include <sstream>
#include <stdexcept>
#include "RecordID.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

RecordID::RecordID() {

	mote_ID = reboot_ID = -1;
}

RecordID::RecordID(int mote_id, int reboot) {

	if (mote_id <=0 || reboot < 0) {

		string msg("incorrect record ID: mote id ");
		msg.append(int2str(mote_id));
		msg.append(" reboot ");
		msg.append(int2str(reboot));

		throw logic_error(msg);
	}

	mote_ID = mote_id, reboot_ID = reboot;
}

const string RecordID::str() const {

	ostringstream os;

	os << *this << flush;

	return os.str();
}

bool operator<(const RecordID& lhs, const RecordID& rhs) {

	bool ret_val;

	if (lhs.mote_ID!=rhs.mote_ID) {

		ret_val = lhs.mote_ID < rhs.mote_ID;
	}
	else {

		ret_val = lhs.reboot_ID < rhs.reboot_ID;
	}

	return ret_val;
}

ostream& operator<<(ostream& out, const RecordID& id) {

	out << "mote id " << id.mote_ID << ", reboot " << id.reboot_ID;

	return out;
}

bool operator==(const RecordID& lhs, const RecordID& rhs) {

	return lhs.mote_ID == rhs.mote_ID && lhs.reboot_ID == rhs.reboot_ID;
}

bool operator!=(const RecordID& lhs, const RecordID& rhs) {

	return !(lhs==rhs);
}

}
