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

#ifndef RECORDSCOUT_HPP_
#define RECORDSCOUT_HPP_

#include <string>
#include <vector>
#include "Line.hpp"
#include "MoteInfo.hpp"
#include "RecordInfo.hpp"
#include "TimeSyncDB.hpp"

namespace sdc {

class RecordScout {

public:

	void read_all_existing();

	const std::vector<MoteInfo>& headers() const;

	const std::vector<RecordInfo>& record_info() const;

	const MoteInfo&   find_moteinfo(  const RecordID& rid) const;

	const RecordInfo& find_recordinfo(const RecordID& rid) const;

	void dump_all() const;

private:

	void clear();

	void read_mote_rdb();

	void push_line(const std::string& buffer);

	void push_back();

	void push_back_record_info();

	void dump_header(const MoteInfo& moteinfo) const;

	void dump_mote(const int pos, const int n) const;

	std::vector<Line> records;

	int mote_id;

	int card_size_in_blocks;

	std::vector<RecordInfo> db;

	std::vector<MoteInfo> header;

	TimeSyncDB motes_online;
};

}

#endif /* RECORDSCOUT_HPP_ */
