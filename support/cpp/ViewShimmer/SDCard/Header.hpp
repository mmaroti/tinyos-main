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

#ifndef HEADER_HPP_
#define HEADER_HPP_

#include <iosfwd>
#include "TypeDefs.hpp"

namespace sdc {

class BlockIterator;

class Header {

public:

	Header() { }

	explicit Header(BlockIterator& itr);

	uint16 data_length() const { return length; }

	uint16 mote() const { return mote_id; }

	uint32 first_block() const { return local_start; }

	void set_timesync_zero();

	bool timesync_differs_from(const Header& other) const;

	void write_timesync_info(std::ostream& ) const;

	friend std::ostream& operator<<(std::ostream& , const Header& );

private:

	uint16 format_id;
	uint16 mote_id;
	uint16 length;
	uint32 local_start; // end of SimpleFileP.nc header
	uint32 local_time;  // struct timesync_info_t starts here
	uint32 remote_time;
	uint32 remote_start;
	uint16 remote_id;
};

}

#endif
