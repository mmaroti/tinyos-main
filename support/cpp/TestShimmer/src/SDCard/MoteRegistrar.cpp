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
#include <stdexcept>
#include "MoteRegistrar.hpp"
#include "Console.hpp"
#include "Constants.hpp"

using namespace std;

namespace sdc {

MoteRegistrar::MoteRegistrar(int mote_id) : mote_ID(mote_id), db(new fstream) {

	bool new_id = read_all_existing_ids();

	if (new_id) {

		Console::registering_moteid_in_db(mote_ID);

		register_id();
	}
	else {

		Console::moteid_already_in_db(mote_ID);
	}
}

bool MoteRegistrar::read_all_existing_ids() {

	bool new_id = true;

	db->open(MOTE_ID_DB, fstream::in);

	if (!db->is_open()) {

		Console::creating_moteid_database();

		ids.push_back(mote_ID);

		new_id = true;
	}
	else {

		Console::reading_moteid_database();

		db->exceptions(fstream::failbit | fstream::badbit);

		new_id = read_file_content();

		db->close();
	}

	db->clear();

	return new_id;
}

void MoteRegistrar::register_id() {

	db->exceptions(fstream::failbit | fstream::badbit);

	db->open(MOTE_ID_DB, fstream::out | fstream::trunc);

	const int n = static_cast<int> (ids.size());

	for (int i=0; i<n-1; ++i) {

		*db << ids.at(i) << '\n';
	}

	*db << ids.at(n-1) << flush;

	db->close();
}

bool MoteRegistrar::read_file_content() {

	int current;

	int previous = 0;

	while (!db->eof()) { // FIXME

		*db >> current;

		if (current <= previous) {

			throw runtime_error("invalid mote id found in the DB");
		}

		if ( current == mote_ID ) {

			return false;
		}

		if (previous < mote_ID && mote_ID < current) {

			ids.push_back(mote_ID);
		}

		ids.push_back(current);

		previous = current;
	}

	if (previous < mote_ID) {

		ids.push_back(mote_ID);
	}

	return true;
}

MoteRegistrar::~MoteRegistrar() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
