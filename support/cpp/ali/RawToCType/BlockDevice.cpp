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
#include <stdexcept>
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

	in->open(source, ios::binary);

	if (!in->good()) {
		clog << "Failed to open file " << source << ", exiting..." << endl;
		exit(FAILED_TO_OPEN_BINARY_FILE);
	}

	in->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

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

	if (i<0 || i>=BLOCK_OFFSET_MAX) {
		throw out_of_range("block index");
	}

	try {

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

	return 0;
}

FileAsBlockDevice::~FileAsBlockDevice() {

	delete in;
	delete[] buffer;
}

#ifdef _WIN32

#include "Win32BlockDevice.h"

Win32BlockDevice::Win32BlockDevice(const char* source) {

	if (BLOCK_SIZE != block_size()) {
		clog << "Implementation is not updated properly: BLOCK_SIZE" << endl;
		exit(IMPLEMENTATION_NOT_UPDATED_PROPERLY);
	}

	const char drive = string(source).at(0);

	card_size = card_size_in_GB(drive);

	if (card_size==0) {
		clog << "Failed to open block device, exiting..." << endl;
		exit(FAILED_TO_OPEN_WIN32_BLOCK_DEVICE);
	}

	if (card_size >= TWO_GB) {
		clog << "Card size is larger than 2GB, exiting..." << endl;
		exit(CARD_SIZE_IS_LARGER_THAN_2GB);
	}
}

const char* Win32BlockDevice::read_block(int i) {

	if (i<0 || i>=MAX_BLOCK_INDEX) {
		throw out_of_range("block index");
	}

	read_device_block(i);
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

	clog << "Win32 block device is not compiled!" << endl;
	exit(WIN32_BLOCK_DEVICE_NOT_COMPILED);
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
