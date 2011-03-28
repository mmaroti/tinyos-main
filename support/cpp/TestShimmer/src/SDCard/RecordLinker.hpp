/* Copyright (c) 2010, 2011 University of Szeged
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

#ifndef RECORDLINKER_HPP_
#define RECORDLINKER_HPP_

#include <iosfwd>
#include <memory>
#include <string>

namespace sdc {

class RecordLinker {

public:

	RecordLinker(const char* filename);

	void write_participant(int mote, int record);

	void write_reference_boot_time(unsigned int global_start_utc);

	void write_record(int mote,
			          int reboot,
			          const std::string& length,
			          unsigned int boot_utc,
			          double skew_1,
			          double offset);

	~RecordLinker();

private:

	RecordLinker(const RecordLinker& );
	RecordLinker& operator=(const RecordLinker& );

	void write_header();
	void write_data();
	void write_line(const std::string& line);
	const std::string mote_time2global_time(unsigned int mote_time) const;


	const std::string record_file_name() const;

	const std::auto_ptr<std::ofstream> out;

	unsigned int global_start;

	int mote;

	int reboot;

	std::string length;

	unsigned int boot_utc;

	double skew_1;

	double offset;
};

}

#endif /* RECORDLINKER_HPP_ */
