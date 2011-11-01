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

#include <fstream>
#include <stdexcept>
#include <limits>
#include <stdint.h>
#include "BlockRelatedConsts.hpp"
#include "FileAsBlockDevice.hpp"
#include "Utility.hpp"

using namespace std;

namespace sdc {

FileAsBlockDevice::FileAsBlockDevice(const char* source)
	: in(new ifstream()), buffer(new char[BLOCK_SIZE])
{

	in->open(source, ios::binary);

	if (!in->good()) {
		string msg("Failed to open file ");
		msg += source;
		throw runtime_error(msg);
	}

	in->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

	in->seekg(0, ios::end);

	// FIXME Is it safe?
	card_size = static_cast<double> (in->tellg()) / GB();

	setBlockOffsetMax();
}

void FileAsBlockDevice::setBlockOffsetMax() {

	// FIXME Is it safe?
	int64_t size_in_bytes = static_cast<int64_t> (in->tellg());

	if (size_in_bytes >= numeric_limits<int>::max() || size_in_bytes < 0) {

		throw runtime_error("Card size is larger than 2GB");
	}

	int size = static_cast<int> (size_in_bytes);

	BLOCK_OFFSET_MAX = size/BLOCK_SIZE;
}

int FileAsBlockDevice::end() const {

	return BLOCK_OFFSET_MAX;
}

const char* FileAsBlockDevice::read_block(int i) {

	if (i<0 || i>=BLOCK_OFFSET_MAX) {
		throw out_of_range("block index");
	}

	try {

		in->seekg(i*BLOCK_SIZE);

		in->read(buffer.get(), BLOCK_SIZE);
	}
	catch (ios_base::failure& ) {

		throw runtime_error(failed_to_read_block(i));
	}

	return buffer.get();
}

double FileAsBlockDevice::size_GB() const {

	return card_size;
}

unsigned long FileAsBlockDevice::error_code() const {

	return 0;
}

FileAsBlockDevice::~FileAsBlockDevice() {
	// Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}


