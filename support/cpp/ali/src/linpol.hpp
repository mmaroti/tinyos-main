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

#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <iosfwd>
#include <string>

class linvars;

class linpol {

public:

	linpol() : val(0), names(0) { }

	explicit linpol(const linvars* const name);

	linpol(const linpol& other);

	linpol& set_constant(double value);

	linpol& set_coefficient(const std::string& name, double value);

	linpol& operator=(const linpol& rhs);

	// Out of necessity, only works with rhs = 0
	linpol& operator=(double rhs);

	linpol& operator+=(const linpol& rhs);

	linpol& operator-=(const linpol& rhs);

	friend const linpol operator-(const linpol& pol);

	friend const linpol operator+(const linpol& lhs, const linpol& rhs);

	friend const linpol operator-(const linpol& lhs, const linpol& rhs);

	friend const linpol operator*(const linpol& lhs, const linpol& rhs);

	~linpol();

	std::ostream& print(std::ostream& os) const;

private:

	void init_if_needed(const linpol& other);

	double* val;

	const linvars* names;
};

std::ostream& operator<<(std::ostream& os, const linpol& a);

#endif
