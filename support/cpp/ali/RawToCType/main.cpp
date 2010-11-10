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
#include <stdexcept>
#include <typeinfo>
#include <memory>
#include "SDCard.hpp"

using namespace std;

namespace {

	enum { SUCCESS, FAILURE };
}

int Main(const char* source) {

	//auto_ptr<SDCard> bd(SDCard::from_win32_drive("D"));

	//cout << "Drive size in GB: " << bd->size_GB() << endl;

	//bd.reset();

	auto_ptr<SDCard> sd(SDCard::from_file(source));

	sd->process_new_measurements();

	return 0;
}

int run(const char* source) {

	int error_code = SUCCESS;

	try {

		Main(source);
	}
	catch (exception& e) {

		clog << e.what() << " (" << typeid(e).name() << ")" << endl;

		error_code = FAILURE;
	}

	return error_code;
}

int main(int argc, char* argv[]) {

	if (argc != 2) {

		clog << "Error: source not specified or too many arguments!" << endl;

		return FAILURE;
	}

	return run(argv[1]);
}
