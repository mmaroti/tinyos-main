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
#include "Sample.hpp"
#include "BlockIterator.hpp"
#include "Constants.hpp"

using namespace std;

const char SEPARATOR    = ',';
const int UINT16MAX = 0xFFFF;

Sample::Sample(BlockIterator& itr) {

	time_stamp = itr.next_uint32();
	seq_num    = itr.next_uint16();
	acc_x      = itr.next_uint16();
	acc_y      = itr.next_uint16();
	acc_z      = itr.next_uint16();
	gyro_x     = itr.next_uint16();
	gyro_y     = itr.next_uint16();
	gyro_z     = itr.next_uint16();
	volt       = itr.next_uint16();
	temp       = itr.next_uint16();
}

ostream& operator<<(ostream& out, const Sample& s) {

	out << s.time_stamp << SEPARATOR;
	out << s.seq_num    << SEPARATOR;
	out << s.acc_x      << SEPARATOR;
	out << s.acc_y      << SEPARATOR;
	out << s.acc_z      << SEPARATOR;
	out << s.gyro_x     << SEPARATOR;
	out << s.gyro_y     << SEPARATOR;
	out << s.gyro_z     << SEPARATOR;
	out << s.volt       << SEPARATOR;
	out << s.temp       << '\n';

	return out;
}

bool Sample::check_reboot(uint16 counter_previous) const {
	return seq_num==1 && counter_previous!=0;
}

int Sample::missing(uint16 counter_previous) const {
	int diff = seq_num-counter_previous;
	if (diff < 0) diff += (UINT16MAX+1);
	return diff-1;
}

int Sample::error_in_ticks(uint32 time_previous) const {
	uint32 expected = time_previous + SAMPLING_RATE;
	return time_stamp - expected;
}
