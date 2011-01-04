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

#ifndef TIMESYNCMERGER_HPP_
#define TIMESYNCMERGER_HPP_

#include <map>
#include <memory>
#include <set>
#include <vector>
#include "RecordPairID.hpp"

namespace sdc {

class FlatFileDB;
class Merger;

typedef std::pair<unsigned int, unsigned int> Pair;
typedef std::map<RecordPairID, std::vector<Pair> > Map;
typedef std::set<RecordID> Set;

class TimeSyncMerger {

public:

	TimeSyncMerger(int mote, int reboot);

	const Map& pairs() const;

	const Set& recordID_of_pairs() const;

	~TimeSyncMerger();

private:

	TimeSyncMerger(const TimeSyncMerger& );
	TimeSyncMerger& operator=(const TimeSyncMerger& );

	void merge();
	void insert(const RecordPairID& id, const std::vector<Pair>& sync_points);
	void insert(const RecordID& other);
	void check_size() const;
	void process_pairs();
	void reset_db_if_needed();

	 // TODO Move these to their own class, this a sort of duplication
	const std::auto_ptr<const FlatFileDB> db1;
	const int mote1;
	const int reboot1;
	const int block1;
	const RecordID rec1;

	std::auto_ptr<FlatFileDB> db2;
	int mote2;
	int reboot2;
	int block2;

	std::auto_ptr<Merger> merger;

	Map result;
	Set otherRecords;
};

}

#endif /* TIMESYNCMERGER_HPP_ */
