/** Copyright (c) 2010, University of Szeged
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
* Author: Ali Baharev
*/

#include <ostream>
#include "Header.hpp"
#include "BlockIterator.hpp"

using namespace std;

namespace sdc {

Header::Header(BlockIterator& itr) {

	format_id = itr.next_uint16();
	mote_id   = itr.next_uint16();
	length    = itr.next_uint16();
	local_start  = itr.next_uint32();
	local_time   = itr.next_uint32();
	remote_time  = itr.next_uint32();
	remote_start = itr.next_uint32();
	remote_id = itr.next_uint16();
}

void Header::set_timesync_zero() {

	remote_start = local_time = remote_time = 0;
	remote_id = 0;
}

bool Header::timesync_differs_from(const Header& h) const {

	const bool differs = (remote_time  != h.remote_time ) ||
						 (remote_start != h.remote_start) ||
						 (local_time   != h.local_time  ) ||
						 (remote_id    != h.remote_id   ) ;

	return differs;
}

void Header::write_timesync_info(std::ostream& out) const {

	out << local_time << '\t' << remote_time  << '\t' ;
	out <<  remote_id << '\t' << remote_start << '\n' << flush;
}

ostream& operator<<(ostream& out, const Header& h) {

	out << "format id:    " << h.format_id << endl;
	out << "mote id:      " << h.mote_id   << endl;
	out << "length:       " << h.length    << endl;
	out << "local start:  " << h.local_start << endl;
	out << "local time:   " << h.local_time << endl;
	out << "remote time:  " << h.remote_time << endl;
	out << "remote start: " << h.remote_start << endl;
	out << "remote id:    " << h.remote_id << endl;

	return out;
}

}
