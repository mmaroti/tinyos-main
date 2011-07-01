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
#include <fstream>
#include <string>
#include <stdexcept>
#include "TimeSyncReader.hpp"
#include "TimeSyncInfo.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

TimeSyncReader::TimeSyncReader(int mote_id, int reboot_id, int first_block)
	: mote(mote_id), reboot(reboot_id), block(first_block), in(new ifstream)
{
	open();

	read_all();

	in->close();
}

void TimeSyncReader::open() {

	string filename = get_filename(mote, reboot, block);

	filename.append(".tsm"); // TODO Move extensions to Constants.hpp

	in->open(filename.c_str());

	if (!in->is_open()) {

		string msg("failed to open file ");

		msg.append(filename);

		throw logic_error(msg);
	}

	cout << "Processing file " << filename << endl;
}

void TimeSyncReader::read_all() {

	string line("dummy");

	while (in->good() && line.size()) {

		getline(*in, line);

		if (line.size()) {

			messages.push_back(TimeSyncInfo(line));
		}
	}

	cout << messages.size() << " messages read" << endl;
}

const std::list<TimeSyncInfo>& TimeSyncReader::messages_as_list() const {

	return messages;
}

TimeSyncReader::~TimeSyncReader() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
