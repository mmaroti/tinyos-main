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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <assert.h>
#include "ErrorCodes.hpp"
#include "BlockDevice.hpp"
#include "BlockRelatedConsts.hpp"

using namespace std;

namespace {

const unsigned int TWO_GB = 1 << 31;

}

FileAsBlockDevice::FileAsBlockDevice(const char* source)
	: BlockDevice(), in(new ifstream()), buffer(new char[BLOCK_SIZE])
{
	in->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

	in->open(source, ios::binary);

	in->seekg(0, ios::end);

	card_size = in->tellg();

	if (card_size >= TWO_GB) {
		clog << "Card size is larger than 2GB, exiting..." << endl;
		exit(CARD_SIZE_IS_LARGER_THAN_2GB);
	}

	card_size /= (TWO_GB/2);

	int size_in_bytes = static_cast<int> (in->tellg());

	BLOCK_OFFSET_MAX = size_in_bytes/BLOCK_SIZE;
}

const char* FileAsBlockDevice::read_block(int i) {

	const char* ret_val = 0;

	try {

		assert(0<=i && i<BLOCK_OFFSET_MAX);

		in->seekg(i*BLOCK_SIZE);

		in->read(buffer, BLOCK_SIZE);

		ret_val = buffer;
	}
	catch (ios_base::failure& excpt) {

		clog << "Warning: exception in read_block() " << excpt.what() << endl;
	}

	return ret_val;
}

double FileAsBlockDevice::size_GB() const {

	return card_size;
}

unsigned long FileAsBlockDevice::error_code() const {
	// TODO Finish implementation
	return 0;
}

FileAsBlockDevice::~FileAsBlockDevice() {

	delete in;
	delete[] buffer;
}



