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
#include <string>
#include <stdexcept>
#include "BlockDevice.hpp"
#include "BlockRelatedConsts.hpp"

using namespace std;

namespace sdc {

FileAsBlockDevice::FileAsBlockDevice(const char* source)
	: BlockDevice(), in(new ifstream()), buffer(new char[BLOCK_SIZE])
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
	card_size = static_cast<double> (in->tellg());

	const unsigned int one = 1;
	const unsigned int GB = one << 30;

	card_size /= GB;

	if (card_size >= 2.0) {
		throw runtime_error("Card size is larger than 2GB");
	}
	// FIXME Is it safe?
	int size_in_bytes = static_cast<int> (in->tellg());

	BLOCK_OFFSET_MAX = size_in_bytes/BLOCK_SIZE;
}

const char* FileAsBlockDevice::read_block(int i) {

	if (i<0 || i>=BLOCK_OFFSET_MAX) {
		throw out_of_range("block index");
	}

	const char* ret_val = 0;

	try {

		in->seekg(i*BLOCK_SIZE);

		in->read(buffer.get(), BLOCK_SIZE);

		ret_val = buffer.get();
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

	return 0;
}

#ifdef _WIN32

#include "Win32BlockDevice.h"

Win32BlockDevice::Win32BlockDevice(const char* source) {

	if (BLOCK_SIZE != block_size()) {
		throw logic_error("Implementation is not updated properly: BLOCK_SIZE");
	}

	const char drive_letter = string(source).at(0);
	wstring path(L"\\\\.\\");
	path += drive_letter;
	path += ':';

	card_size = card_size_in_GB(path.c_str());

	if (card_size==0) {
		string msg("Failed to open block device: ");
		msg += source;
		throw runtime_error(msg);
	}

	if (card_size >= 2.0) {
		close_device();
		throw runtime_error("Card size is larger than 2GB");
	}
}

const char* Win32BlockDevice::read_block(int i) {

	if (i<0 || i>=MAX_BLOCK_INDEX) {
		throw out_of_range("block index");
	}

	return read_device_block(i);
}

double Win32BlockDevice::size_GB() const {

	return card_size;
}

unsigned long Win32BlockDevice::error_code() const {

	return error_code();
}

Win32BlockDevice::~Win32BlockDevice() {

	close_device();
}

#else

Win32BlockDevice::Win32BlockDevice(const char* source) {

	throw logic_error("Win32 block device is not compiled!");
}

const char* Win32BlockDevice::read_block(int i) {

	return 0;
}

double Win32BlockDevice::size_GB() const {

	return 0;
}

unsigned long Win32BlockDevice::error_code() const {

	return 0;
}

Win32BlockDevice::~Win32BlockDevice() {

}

#endif

}

