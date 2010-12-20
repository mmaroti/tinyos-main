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

#ifndef MOTEREGISTRAR_HPP_
#define MOTEREGISTRAR_HPP_

#include <iosfwd>
#include <memory>
#include <vector>

namespace sdc {

class MoteID_Size {

public:

	MoteID_Size();

	MoteID_Size(int mote_id, int size_in_blocks);

	int mote_id() const;

	int size_in_blocks() const;

	friend std::istream& operator>>(std::istream& , MoteID_Size& );

private:

	int id;

	int end;
};

std::ostream& operator<<(std::ostream& , const MoteID_Size& );

class MoteRegistrar {

public:

	MoteRegistrar(int mote_id, int size_in_blocks);

	~MoteRegistrar();

	static const std::vector<MoteID_Size> existing_ids();

private:

	MoteRegistrar(const MoteRegistrar& );
	MoteRegistrar& operator=(const MoteRegistrar& );

	void push_back();

	void read_all_existing_ids();

	void read_file_content();

	void check_size(const MoteID_Size& current) const;

	void process(MoteID_Size& previous, const MoteID_Size& current);

	void register_id();

	const int mote_ID;

	const int size_in_blocks;

	const std::auto_ptr<std::fstream> db;

	std::vector<MoteID_Size> motes;

	bool new_id;
};

}

#endif /* MOTEREGISTRAR_HPP_ */
