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

#include <fstream>
#include <stdexcept>
#include "Writer.hpp"
#include "Header.hpp"
#include "Sample.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

Writer::Writer() : samples(new ofstream), timesync(new ofstream) {

	samples->exceptions( ofstream::failbit | ofstream::badbit);
	timesync->exceptions(ofstream::failbit | ofstream::badbit);
}

void Writer::start_new_record(int mote_id, int reboot_id, int first_block) {

	string fsamples(get_filename(mote_id, reboot_id, first_block));
	string ftimesync(fsamples);

	samples->open(  fsamples.append(".csv").c_str());
	timesync->open(ftimesync.append(".tsm").c_str());
}

bool Writer::is_open() const {

	const bool open = samples->is_open();

	if (open!=timesync->is_open()) {
		throw logic_error("illegal state of output files");
	}

	return open;
}

void Writer::write_time_sync_info(const Header& h) {

	h.write_timesync_info(*timesync);
}

void Writer::write(const Sample& s) {

	*samples << s;
}

void Writer::flush() {

	samples->flush();
	timesync->flush();
}

void Writer::close() {

	samples->close();
	timesync->close();
}

Writer::~Writer() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
