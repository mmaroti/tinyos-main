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
#include <stdexcept>
#include "VirtualMoteID.hpp"
#include "TimeSyncInfo.hpp"

using namespace std;

namespace sdc {

VirtualMoteID::VirtualMoteID() {

	mote_ID = start_at_block = 0;
}

VirtualMoteID::VirtualMoteID(int mote_id, int first_block) {

	mote_ID = mote_id;

	start_at_block = first_block;
}

VirtualMoteID::VirtualMoteID(const TimeSyncInfo& msg) {

	if (msg.remote_id==0) {

		throw logic_error("inconsistent messages should have been removed");
	}

	mote_ID        = msg.remote_id;

	start_at_block = msg.remote_start;
}

int VirtualMoteID::mote_id() const {

	return mote_ID;
}

int VirtualMoteID::first_block() const {

	return start_at_block;
}

ostream& operator<<(ostream& out, const VirtualMoteID& id) {

	out << "mote id " << id.mote_ID << ", first block " << id.start_at_block << flush;

	return out;
}

bool operator==(const VirtualMoteID& lhs, const VirtualMoteID& rhs) {

	return lhs.mote_ID==rhs.mote_ID && lhs.start_at_block==rhs.start_at_block;
}

bool operator!=(const VirtualMoteID& lhs, const VirtualMoteID& rhs) {

	return !(lhs==rhs);
}

}
