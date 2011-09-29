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

#include <ctime>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "Constants.hpp"
#include "BlockRelatedConsts.hpp"
#include "Utility.hpp"

using namespace std;

typedef ostringstream oss;

namespace sdc {

const string ticks2time(unsigned int t) {

	oss os;

	unsigned int hour, min, sec, milli;

	hour = t/(3600*TICKS_PER_SEC);
	t =    t%(3600*TICKS_PER_SEC);

	min = t/(60*TICKS_PER_SEC);
	t   = t%(60*TICKS_PER_SEC);

	sec = t/TICKS_PER_SEC;
	t   = t%TICKS_PER_SEC;

	milli = static_cast<unsigned int> ( t/(TICKS_PER_SEC/1000.0) );

	os << setfill('0') << setw(2) << hour << ":";
	os << setfill('0') << setw(2) << min  << ":";
	os << setfill('0') << setw(2) << sec  << ".";
	os << setfill('0') << setw(3) << milli<< flush;

	return os.str();
}

const string current_time() {

	time_t t;
	time(&t);
	return string(ctime(&t));
}

const string get_filename(int mote_id, int reboot_id, int first_block) {

	oss os;

	os << 'm' << setfill('0') << setw(3) << mote_id << '_';
	os << 'r' << setfill('0') << setw(3) << reboot_id << '_';
	os << 'b' << first_block;

	os.flush(); // TODO Is it needed?

	return os.str();
}

string time_to_filename() {

	string time_stamp(current_time().substr(4, 20));

	time_stamp.erase(3, 1);
	time_stamp.at(5) = '_';
	time_stamp.erase(8, 1);
	time_stamp.erase(10, 1);
	time_stamp.at(12) = '_';
	time_stamp.erase(13, 2);

	return time_stamp;
}

unsigned int length_in_ticks(int first_block, int last_block) {

	int n_blocks = last_block-first_block+1;

	int n_samples = n_blocks*MAX_SAMPLES-1;

	return n_samples*SAMPLING_RATE;
}

const string recorded_length(int first_block, int last_block) {

	unsigned int length = length_in_ticks(first_block, last_block);

	return ticks2time(length);
}

double remaining_GB_(double card_size, int last_block) {

	double used_bytes = last_block*BLOCK_SIZE ;

	double used_GB = used_bytes/GB();

	return card_size-used_GB;
}

double remaining_hours_(double card_size, int last_block) {

	double remaining = remaining_GB_(card_size, last_block);

	double remaining_blocks = (remaining/BLOCK_SIZE*GB());

	double remaining_samples = remaining_blocks*MAX_SAMPLES;

	double remaining_sec = (remaining_samples*SAMPLING_RATE)/TICKS_PER_SEC;

	return remaining_sec/3600;
}

const string double2str_2decimals(double x) {

	oss os;

	os << setprecision(2) << fixed << x << flush;

	return os.str();
}

const string remaining_GB(double card_size, int last_block) {

	return double2str_2decimals( remaining_GB_(card_size, last_block) );
}

const string remaining_hours(double card_size, int last_block) {

	return double2str_2decimals( remaining_hours_(card_size, last_block) );
}

int round(double x) {

	return static_cast<int> (floor(x+0.5));
}

int recorded_length_in_ms(int first_block, int last_block) {

	double length = length_in_ticks(first_block, last_block);

	return round(length/(TICKS_PER_SEC/1024.0));
}

const string failed_to_read_block(int i) {

	oss os;

	os << "Failed to read block " << i << flush;

	return os.str();
}


const std::string rdb_file_name(int mote_id) {

	oss os;

	os << 'm' << setfill('0') << setw(3) << mote_id << ".rdb" << flush;

	return os.str();
}

const std::string int2str(int i) {

	oss os;

	os << i << flush;

	return os.str();
}

unsigned int GB() {

	unsigned int one = 1;

	return (one << 30);
}

}
