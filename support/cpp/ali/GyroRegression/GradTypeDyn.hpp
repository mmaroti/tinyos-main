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

#ifndef GRADTYPEDYN_HPP_
#define GRADTYPEDYN_HPP_

#include <iosfwd>

namespace gyro {

void set_number_of_variables(int n);

class GradType {

public:

	GradType();

	GradType(double constant);

	GradType(const GradType& other);

	friend void init_vars(GradType var[], const double x[]);

	friend const GradType operator-(const GradType& x);

	GradType& operator=(const GradType& rhs);

	GradType& operator=(double rhs);

	GradType& operator+=(const GradType& x);

	GradType& operator+=(double x);

	GradType& operator-=(const GradType& x);

	GradType& operator-=(double x);

	GradType& operator*=(const GradType& x);

	GradType& operator*=(double x);

	GradType& operator/=(double x);

	friend const GradType operator/(const GradType& x, const GradType& y);

	friend const GradType operator/(const double x, const GradType& y);

	friend const GradType pow(const GradType& x, int n);

	friend const GradType sqrt(const GradType& x);

	double value() const;

	const double* gradient() const;

	int size() const;

	std::ostream& print(std::ostream& os) const;

	~GradType();

private:

	void copy(const GradType& other);

	void init(double value);

	double f;

	double* const g;
};

const GradType operator+(const GradType& x, const GradType& y);

const GradType operator+(const double x, const GradType& y);

const GradType operator+(const GradType& x, const double y);

const GradType operator-(const GradType& x, const GradType& y);

const GradType operator-(const double x, const GradType& y);

const GradType operator-(const GradType& x, const double y);

const GradType operator*(const GradType& x, const GradType& y);

const GradType operator*(const double x, const GradType& y);

const GradType operator*(const GradType& x, const double y);

std::ostream& operator<<(std::ostream& os, const GradType& x);

}

#endif
