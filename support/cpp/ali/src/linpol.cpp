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

#include <cassert>
#include <iostream>
#include "linvars.hpp"
#include "linpol.hpp"
#include "utility.hpp"

using namespace std;

linpol::linpol(const linvars* const vars) {

	const int n = vars->size();

	val = new double[n];

	names = vars;

	for (int i=0; i<n; ++i)
		val[i] = 0.0;
}

linpol::linpol(const linpol& other) : val(0), names(0) {

	if (other.names != 0) {

		const int n = other.names->size();

		val = new double[n];

		names = other.names;

		for (int i=0; i<n; ++i)
			val[i] = other.val[i];
	}
}

void linpol::init_if_needed(const linpol& other) {

	assert(other.names != 0);

	if (names==0) {

		const int n = other.names->size();

		val = new double[n];

		names = other.names;

		for (int i=0; i<n; ++i)
			val[i] = 0.0;
	}

	assert(names==other.names);
}

linpol& linpol::operator=(const linpol& rhs) {

	if (rhs.names != 0) {

		init_if_needed(rhs);

		const int n = rhs.names->size();

		for (int i=0; i<n; ++i)
			val[i] = rhs.val[i];
	}

	return *this;
}

// TODO Automatic type conversion?
linpol& linpol::operator=(double rhs) {

	if (rhs != 0.0)
		error("operator= works only with rhs = 0");

	if (val !=0 ) {

		const int n = names->size();

		for (int i=0; i<n; ++i)
			val[i] = 0.0;
	}

	return *this;
}

linpol& linpol::operator+=(const linpol& rhs) {

	init_if_needed(rhs);

	const int n = rhs.names->size();

	for (int i=0; i<n; ++i)
		val[i] += rhs.val[i];

	return *this;
}

const linpol operator+(const linpol& lhs, const linpol& rhs) {

	linpol result(lhs);

	result += rhs;

	return result;
}

linpol& linpol::operator-=(const linpol& rhs) {

	init_if_needed(rhs);

	const int n = rhs.names->size();

	for (int i=0; i<n; ++i)
		val[i] -= rhs.val[i];

	return *this;
}

const linpol operator-(const linpol& lhs, const linpol& rhs) {

	linpol result(lhs);

	result -= rhs;

	return result;
}

const linpol operator-(const linpol& pol) {

	assert(pol.names!=0);

	linpol result(pol.names);

	const int n = pol.names->size();

	for (int i=0; i<n; ++i)
		result.val[i] = - pol.val[i];

	return result;
}

const linpol operator*(const linpol& lhs, const linpol& rhs) {

	assert ( (lhs.names != 0) && (rhs.names != 0));

	linpol result(lhs.names);

	// c = a*b
	const double* const a = lhs.val;
	const double* const b = rhs.val;

	double* const c = result.val;

	const double a0 = a[0];
	const double b0 = b[0];

	c[0] = a0*b0;

	const int n = result.names->size();

	for (int i=1; i<n; ++i)
		c[i] = a0*b[i] + b0*a[i];

	return result;
}

linpol::~linpol() {

	delete[] val;
}

linpol& linpol::set_constant(double value) {

	assert( names!=0 );

	val[0] = value;

	return *this;
}

linpol& linpol::set_coefficient(const string& name, double value) {

	assert( names!=0 );

	const int index = names->find_index(name);

	const int size = names->size();

	assert ( (0<index) && (index<size) );

	val[index] = value;

	return *this;
}

std::ostream& linpol::print(std::ostream& os) const {

	if (names==0) {

		os << "not initialized" << endl;
	}
	else {

		const int size = names->size();

		const string* const vars = names->varnames();

		for (int i=0; i<size; ++i)
			os << vars[i] << '\t' << val[i] << endl;
	}

	os << endl;

	return os;
}

std::ostream& operator<<(std::ostream& os, const linpol& a) {

	return a.print(os);
}
