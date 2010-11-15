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

#include <stdexcept>
#include "Win32BlockDevice.hpp"
#ifdef _WIN32
#include "BlockRelatedConsts.hpp"
#include "Utility.hpp"
#include "WinBlockDevice.h"
#endif

using namespace std;

namespace sdc {

#ifdef _WIN32

Win32BlockDevice::Win32BlockDevice(const char* source) : buffer(new char[BLOCK_SIZE]) {

	const char drive_letter = string(source).at(0);
	wstring path(L"\\\\.\\");
	path += drive_letter;
	path += ':';

	card_size = card_size_in_GB(path.c_str(), &hDevice);

	if (card_size==0) {
		string msg("Failed to open block device: ");
		msg += source;
		throw runtime_error(msg);
	}

	if (card_size >= 2.0) {
		close_device(&hDevice);
		throw runtime_error("Card size is larger than 2GB");
	}
}

const char* Win32BlockDevice::read_block(int i) {

	if (i<0 || i>=MAX_BLOCK_INDEX) {
		throw out_of_range("block index");
	}

	unsigned int size = static_cast<unsigned int> (BLOCK_SIZE);

	const char* const block = read_device_block(&hDevice, i, buffer.get(), size);

	if (block==0) {

		throw runtime_error(failed_to_read_block(i));
	}

	return block;
}

double Win32BlockDevice::size_GB() const {

	return card_size;
}

unsigned long Win32BlockDevice::error_code() const {

	return error_code();
}

Win32BlockDevice::~Win32BlockDevice() {

	close_device(&hDevice);
}

#else

Win32BlockDevice::Win32BlockDevice(const char* ) {

	throw logic_error("Win32 block device is not compiled!");
}

const char* Win32BlockDevice::read_block(int ) {

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
