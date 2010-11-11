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

#ifndef SDCARDIMPL_HPP_
#define SDCARDIMPL_HPP_

#include <iosfwd>
#include <memory>
#include "TypeDefs.hpp"

namespace sdc {

class BlockDevice;
class BlockChecker;
class BlockIterator;
class Tracker;

class SDCardImpl {

public:

	explicit SDCardImpl(BlockDevice* source);

	void process_new_measurements();

	double size_GB() const;

	~SDCardImpl();

private:

	SDCardImpl(const SDCardImpl& );
	SDCardImpl& operator=(const SDCardImpl& );

	void print_start_banner() const;
	void print_finished_banner() const;
	void close_out_if_open();
	void create_new_file();
	bool reboot(const int sample_in_block);
	void check_sample(const int sample_in_block);
	void write_samples(BlockIterator& itr);
	bool process_block(const char* block);
	void init_tracker();

	const std::auto_ptr<BlockDevice> device;
	const std::auto_ptr<std::ofstream> out;
	std::auto_ptr<Tracker> tracker;
	std::auto_ptr<BlockChecker> check;
	uint32 time_start;
	int block_offset;
	int reboot_seq_num;
};

}

#endif
