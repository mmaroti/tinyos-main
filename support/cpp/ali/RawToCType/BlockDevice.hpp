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

#ifndef BLOCKDEVICE_HPP_
#define BLOCKDEVICE_HPP_

#include <iosfwd>
#include <memory>

namespace sdc {

class BlockDevice {

public:

	BlockDevice() { }

	virtual const char* read_block(int i) = 0;

	virtual double size_GB() const = 0;

	virtual unsigned long error_code() const = 0;

	virtual ~BlockDevice() { }

private:

	BlockDevice(const BlockDevice& );

	BlockDevice& operator=(const BlockDevice& );

};

class FileAsBlockDevice : public BlockDevice {

public:

	explicit FileAsBlockDevice(const char* source);

private:

	virtual const char* read_block(int i);

	virtual double size_GB() const;

	virtual unsigned long error_code() const;

	virtual ~FileAsBlockDevice();

	const std::auto_ptr<std::ifstream> in;

	const std::auto_ptr<char> buffer;

	int BLOCK_OFFSET_MAX;

	double card_size;
};

class Win32BlockDevice : public BlockDevice {

public:

	explicit Win32BlockDevice(const char* source);

private:

	virtual const char* read_block(int i);

	virtual double size_GB() const;

	virtual unsigned long error_code() const;

	virtual ~Win32BlockDevice();

	double card_size;
};

}

#endif
