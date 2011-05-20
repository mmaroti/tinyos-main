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

#ifndef MERGER_HPP_
#define MERGER_HPP_

#include <list>
#include <map>
#include <vector>
#include "VirtualMoteID.hpp"

namespace sdc {

class TimeSyncInfo;

typedef std::list<TimeSyncInfo> List;
typedef std::pair<unsigned int, unsigned int> Pair;

class Merger {

public:

	Merger(const VirtualMoteID& vmote, const List& messages, int length_in_ms);

	bool set_next();

	bool mote2_id_changed() const;

	int mote2_id() const;

	int block2() const;

	void set_mote2_messages(const List& messages_mote2, int length2_in_ms);

	int merge();

	const std::vector<Pair> results_in_mote_id_order() const;

private:

	Merger(const Merger& );
	Merger& operator=(Merger& );

	typedef std::pair<const unsigned int, unsigned int> CPair;
	typedef std::map<unsigned int, unsigned int> Map;
	typedef Map::iterator mi;
	typedef Map::const_iterator cmi;

	void copy_in_reveresed_order(std::vector<Pair>& pairs) const;
	void drop_inconsistent(List& messages);
	void drop_not_from_mote1();
	void drop_wrong_offsets();
	void handle_conflicting_keys(mi& pos, const Pair& sync_point);
	void init_for_mote2();
	int  initial_offset() const;
	void insert(const Pair& sync_point);
	void log_msg_loss(const List& msg, const VirtualMoteID& vmid) const;
	void log_size_before_merge() const;
	int  offset(const CPair& p) const;
	bool sufficient_size();
	bool wrong_offset(const CPair& time_pair, int& previous_offset) const;

	const VirtualMoteID vmote1;
	const int length1;

	List mote1;
	List mote2;
	List temp;
	Map merged;

	VirtualMoteID vmote2;
	int length2;
	bool mote2_id_new;
};

}

#endif /* MERGER_HPP_ */
