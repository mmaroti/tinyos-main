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
#include "TimeSyncInfo.hpp"
#include "TimeSyncConsts.hpp"
#include "Utility.hpp"

using namespace std;

typedef istringstream iss;

namespace sdc {

TimeSyncInfo::TimeSyncInfo() {

	local_time = remote_time = remote_id = remote_start = 0;
}

TimeSyncInfo::TimeSyncInfo(const string& line_from_file) {

	iss in(line_from_file);

	in.exceptions(iss::failbit | iss::badbit);

	in >> local_time;
	in >> remote_time;
	in >> remote_id;
	in >> remote_start;
}

bool TimeSyncInfo::consistent() const {

	return (local_time  > 0 &&
			remote_time > 0 &&
			remote_id   > 0 &&
			remote_start >= 0 );
}

int TimeSyncInfo::lost_messages_since(const TimeSyncInfo& other) const {

	if (remote_id != other.remote_id || remote_start != other.remote_start) {

		throw logic_error("trying to compare messages from different records");
	}

	double diff = static_cast<double> (remote_time) - other.remote_time;

	if (diff <= 0) {

		throw runtime_error("cannot handle this type of error (remote_time)");
	}

	return round(diff/TIMESYNC_MSG_RATE) - 1;
}

const Pair TimeSyncInfo::time_pair() const {

	return Pair(local_time, remote_time);
}

const Pair TimeSyncInfo::reversed_time_pair() const {

	return Pair(remote_time, local_time);
}

ostream& operator<<(ostream& out, const TimeSyncInfo& msg) {

	out << "local_time:   " << msg.local_time   << endl;
	out << "remote_time:  " << msg.remote_time  << endl;
	out << "remote_id:    " << msg.remote_id    << endl;
	out << "remote_start: " << msg.remote_start << flush;

	return out;
}

}
