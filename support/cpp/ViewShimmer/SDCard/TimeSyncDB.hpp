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

#ifndef TIMESYNCDB_HPP_
#define TIMESYNCDB_HPP_

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include "VirtualMoteID.hpp"

namespace sdc {

class TimeSyncDB {

typedef std::map<VirtualMoteID, std::string> Map;
typedef std::pair<Map::iterator, bool> Pair;

public:

	TimeSyncDB() { }

	void read_all();

	const std::string date(const VirtualMoteID& vmote_id) const;

	~TimeSyncDB();

private:

	TimeSyncDB(const TimeSyncDB& );
	TimeSyncDB& operator=(const TimeSyncDB& );

	void grab_line();

	void parse_line();

	void parse();

	void push();

	std::auto_ptr<std::ifstream> in;
	Map motes;
	VirtualMoteID vmote_id;
	std::string date_seen;
	std::string line;
};

}

#endif /* TIMESYNCDB_HPP_ */
