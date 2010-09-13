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
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "CompileTimeConstants.hpp"
#include "BoundReader.hpp"

using namespace std;

namespace gyro {

BoundReader::BoundReader(const char* filename) {

	file_id_ = CONFIG_FILE_NOT_FOUND;

	elem_lb_ = -2.5;
	elem_ub_ =  2.5;

	offset_lb_ = -2.5;
	offset_ub_ =  2.5;

	try {

		ifstream in(filename);

		if (in.good()) {

			file_id_ = CONFIG_FILE_HAS_NO_ID;

			read_all_lines(in);
		}
	}
	catch (...) {
		cerr << "Unknown error in " << __FILE__ << ", " << __LINE__ << "!" << endl;
		exit(ERROR_UNKNOWN);
	}
}

void BoundReader::read_all_lines(ifstream& in) {

	while (in.good()) {

		string line;

		getline(in, line);

		process_line(line);
	}
}

void BoundReader::process_line(const string& line) {

	if ((line.size()==0) || (line.at(0)=='#')) {
		return;
	}

	istringstream s(line);

	string key;

	s >> key;

	if (key.size()==0) {
		// Empty lines with whitespaces are allowed
		return;
	}

	double value(0.0); // 0 cannot be a bound or a valid file ID!

	s >> value;

	set_value(key, value);

}

void BoundReader::set_value(const string& key, double value) {

	if (value==0.0) {
		cerr << "Error reading the configuration file, incorrect value ";
		cerr << key << '\t' << value << endl;
		exit(ERROR_READING_CONFIG);
	}

	if (key == "elem_lb") {
		elem_lb_ = value;
	}
	else if (key == "elem_ub") {
		elem_ub_ = value;
	}
	else if (key == "offset_lb") {
		offset_lb_ = value;
	}
	else if (key == "offset_ub") {
		offset_ub_ = value;
	}
	else if (key == "file_id") {
		file_id_ = static_cast<int> (value);
	}
	else {
		cerr << "Error reading the configuration file, unknown key " << key << endl;
		exit(ERROR_READING_CONFIG);
	}
}

void BoundReader::show(ostream& out) {

	out << "file_id" << '\t' << file_id_ << endl;
	out << "elem_lb" << '\t' << elem_lb_ << endl;
	out << "elem_ub" << '\t' << elem_ub_ << endl;
	out << "offset_lb" << '\t' << offset_lb_ << endl;
	out << "offset_ub" << '\t' << offset_ub_ << endl;
}

}

