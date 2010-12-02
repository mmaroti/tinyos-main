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
#include <iomanip>
#include "Console.hpp"
#include "BlockRelatedConsts.hpp"
#include "Constants.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

void Console::start(double card_size, int mote_id, int first_block, int reboot) const {

	cout << "Card size is ";
	cout << setprecision(2) << fixed << card_size << " GB" << endl;
	cout << "Mote ID: " << mote_id << endl;
	cout << "Starting at block " << first_block << ", ";
	cout << "previous reboot sequence number is " << reboot << endl;
	cout << "---------------------------------------------------------" << endl;
}

void Console::finished(double card_size, int last_block) const {

	const unsigned int GB = 1 << 30;

	const double used_bytes = last_block*BLOCK_SIZE ;

	const double used_GB = used_bytes/GB;

	const double remaining = card_size-used_GB;

	const double remaining_blocks = (remaining/BLOCK_SIZE*GB);

	const double remaining_samples = remaining_blocks*MAX_SAMPLES;

	const double remaining_sec = (remaining_samples*SAMPLING_RATE)/TICKS_PER_SEC;

	cout << "Remaining " << remaining << " GB, approximately ";
	cout << setprecision(2) << fixed << remaining_sec/3600 << " hours" << endl;
	cout << "Finished!" << endl;
}

void Console::record_start(int reboot_id, int first_block) const {

	cout << "Reboot " << reboot_id << " at block " << first_block << endl;
}

void Console::record_end(int last_block, uint32 length) const {

	cout << "Record length " << ticks2time(length) << ", last block ";
	cout << last_block << endl;
	cout << "---------------------------------------------------------" << endl;
}


void Console::error_impossible_state() const {

	cout << "Error: impossible state detected, either the database is ";
	cout << "corrupted or it is bug, trying to recover..." << endl;
}

}
