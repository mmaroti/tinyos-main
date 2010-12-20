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
#include "Utility.hpp"

using namespace std;

namespace sdc {

MoteID_Size::MoteID_Size() {

	id  =  0;
	end = -1;
}

MoteID_Size::MoteID_Size(int mote_id, int size_in_blocks) {

	id  = mote_id;
	end = size_in_blocks;
}

int MoteID_Size::mote_id() const {

	return id;
}

int MoteID_Size::size_in_blocks() const {

	return end;
}

ostream& operator<<(ostream& out, const MoteID_Size& m) {

	out << m.mote_id() << '\t' << m.size_in_blocks();

	return out;
}

istream& operator>>(istream& in, MoteID_Size& m) {

	in >> m.id;
	in >> m.end;

	return in;
}

MoteRegistrar::MoteRegistrar(int mote_id, int size)
: mote_ID(mote_id), size_in_blocks(size), db(new fstream)
{

	new_id = false;

	read_all_existing_ids();

	if (new_id) {

		Console::registering_moteid_in_db(mote_ID);

		register_id();
	}
	else {

		Console::moteid_already_in_db(mote_ID);
	}
}

void MoteRegistrar::push_back() {

	motes.push_back(MoteID_Size(mote_ID, size_in_blocks));

	new_id = true;
}

void MoteRegistrar::read_all_existing_ids() {

	db->open(MOTE_ID_DB, fstream::in);

	if (!db->is_open()) {

		Console::creating_moteid_database();

		push_back();
	}
	else {

		Console::reading_moteid_database();

		db->exceptions(fstream::failbit | fstream::badbit);

		read_file_content();

		db->close();
	}

	db->clear();
}

void MoteRegistrar::register_id() {

	db->exceptions(fstream::failbit | fstream::badbit);

	db->open(MOTE_ID_DB, fstream::out | fstream::trunc);

	const int n = static_cast<int> (motes.size());

	for (int i=0; i<n-1; ++i) {

		*db << motes.at(i) << '\n';
	}

	*db << motes.at(n-1) << flush;

	db->close();
}

void MoteRegistrar::read_file_content() {

	MoteID_Size previous(0, -1);

	while (!db->eof()) {

		MoteID_Size current;

		*db >> current;

		process(previous, current);
	}

	if (previous.mote_id() < mote_ID) {

		push_back();
	}
}

void MoteRegistrar::check_size(const MoteID_Size& current) const {

	if (current.size_in_blocks()!=size_in_blocks) {

		string msg("invalid size for mote ");

		msg.append(int2str(mote_ID));

		throw runtime_error(msg);
	}
}

void MoteRegistrar::process(MoteID_Size& previous, const MoteID_Size& current) {

	if (current.mote_id() <= previous.mote_id()) {

		throw runtime_error("invalid mote id found in the DB");
	}

	if ( current.mote_id() == mote_ID ) {

		check_size(current);

		new_id = false;
	}

	if (previous.mote_id() < mote_ID && mote_ID < current.mote_id()) {

		push_back();
	}

	motes.push_back(current);

	previous = current;
}

MoteRegistrar::~MoteRegistrar() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

const vector<MoteID_Size> MoteRegistrar::existing_ids() {

	vector<MoteID_Size> ids;

	fstream db(MOTE_ID_DB, fstream::in);

	if (!db.is_open()) {

		return ids;
	}

	db.exceptions(fstream::failbit | fstream::badbit);

	MoteID_Size m;

	while (!db.eof()) {

		db >> m;

		ids.push_back(m);
	}

	return ids;
}

}
