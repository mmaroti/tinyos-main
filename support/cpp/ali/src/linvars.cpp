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
#include "linvars.hpp"
#include "utility.hpp"

using namespace std;

linvars::linvars(const string arr[], int length) {

	if (length < 1)
		error("there should be at least one variable");

	n = length+1;

	vars = new string[n];

	vars[0] = string("");

	for (int i=1; i<n; ++i)
		vars[i] = arr[i-1];

}

linvars::~linvars() {

	delete[] vars;
}

int linvars::size() const { return n; }

int linvars::find_index(const string & name) const {

	for (int i=1; i<n; ++i) {

		if (vars[i] == name)
			return i;
	}

	error("variable name not found, maybe a typo");
	// Control never reaches this line
	return -1;
}

ostream& linvars::print(ostream & os) const {

	for (int i=0; i<n; ++i)
		os << vars[i] << endl;
	os << endl;

	return os;
}

const string* linvars::varnames() const {
	return vars;
}

ostream& operator<<(ostream & os, const linvars & a) {

	return a.print(os);
}


