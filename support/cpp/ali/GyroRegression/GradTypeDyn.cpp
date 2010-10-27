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

#include <cmath>
#include <ostream>
#include <assert.h>
#include "GradTypeDyn.hpp"

namespace {

	int N = 0;
}

namespace gyro {

void set_number_of_variables(int n) {

	N = n;
}

GradType::GradType() : g(new double[N]) {

	assert(N>0);
}

GradType::GradType(double constant) : g(new double[N]) {

	assert(N>0);
	init(constant);
}

GradType::GradType(const GradType& other) : g(new double[N]) {

	assert(N>0);
	copy(other);
}


GradType::~GradType() {

	delete[] g;
}

GradType& GradType::operator=(const GradType& rhs) {

	copy(rhs);

	return *this;
}

GradType& GradType::operator=(double rhs) {

	init(rhs);

	return *this;
}

GradType& GradType::operator+=(double x) {

	f += x;

	return *this;
}

GradType& GradType::operator-=(double x) {

	f -= x;

	return *this;
}

double GradType::value() const {

	return f;
}

const double* GradType::gradient() const {

	return g;
}

int GradType::size() const {

	return N;
}

std::ostream& GradType::print(std::ostream& os) const {

	os << f << std::endl;

	os << g[0];

	for (int i=1; i<N; ++i) {

		os << '\t' << g[i];
	}

	os << std::endl;

	return os;
}

std::ostream& operator<<(std::ostream& os, const GradType& x) {

	return x.print(os);
}

void GradType::init(double value) {

	f = value;

	for (int i=0; i<N; ++i) {

		g[i] = 0.0;
	}
}

void GradType::copy(const GradType& other) {
	f = other.f;

	for (int i=0; i<N; ++i) {

		g[i] = other.g[i];
	}
}

void init_vars(GradType var[], const double x[]) {

	for (int i=0; i<N; ++i) {

		var[i] = x[i];

		var[i].g[i] = 1.0;
	}
}

const GradType operator-(const GradType& x) {

	GradType z;

	z.f = -x.f;

	for (int i=0; i<N; ++i) {

		z.g[i] = -x.g[i];
	}

	return z;
}

GradType& GradType::operator+=(const GradType& x) {

	f += x.f;

	for (int i=0; i<N; ++i) {

		g[i] += x.g[i];
	}

	return *this;
}

const GradType operator+(const GradType& x, const GradType& y) {

	GradType z(x);

	return z+=y;
}

const GradType operator+(const double x, const GradType& y) {

	GradType z(y);

	return z+=x;
}

const GradType operator+(const GradType& x, const double y) {

	GradType z(x);

	return z+=y;
}

GradType& GradType::operator-=(const GradType& x) {

	f -= x.f;

	for (int i=0; i<N; ++i) {

		g[i] -= x.g[i];
	}

	return *this;
}

const GradType operator-(const GradType& x, const GradType& y) {

	GradType z(x);

	return z-=y;
}


const GradType operator-(const double x, const GradType& y) {

	return x+(-y);
}

const GradType operator-(const GradType& x, const double y) {

	GradType z(x);

	return z-=y;
}

GradType& GradType::operator*=(double x) {

	f *= x;

	for (int i=0; i<N; ++i) {

		g[i] *= x;
	}

	return *this;
}

GradType& GradType::operator/=(double x) {

	f /= x;

	for (int i=0; i<N; ++i) {

		g[i] /= x;
	}

	return *this;
}

const GradType operator*(const double x, const GradType& y) {

	GradType z(y);

	return z*=x;
}


const GradType operator*(const GradType& x, const double y) {

	return y*x;
}

GradType& GradType::operator*=(const GradType& x) {

	const double y_f = f;

	(*this) *= x.f;

	for (int i=0; i<N; ++i) {

		g[i] += y_f*x.g[i];
	}

	return *this;
}

const GradType operator*(const GradType& x, const GradType& y) {

	GradType z(y);

	return z *= x;
}

const GradType operator/(const GradType& x, const GradType& y) {

	GradType z;

	z.f = x.f/y.f;

	for (int i=0; i<N; ++i) {

		z.g[i] = (x.g[i] - z.f*y.g[i]) / y.f;
	}

	return z;
}

const GradType operator/(const double x, const GradType& y) {

	GradType z;

	z.f = x/y.f;

	const double p = -z.f/y.f;

	for (int i=0; i<N; ++i) {

		z.g[i] = p*y.g[i];
	}

	return z;
}

const GradType operator/(const GradType& x, const double y) {

	return x*(1.0/y);
}

const GradType pow(const GradType& x, int n) {

	GradType z;

	z.f = std::pow(x.f, n);

	const double h = n*std::pow(x.f,n-1);

	for (int i=0; i<N; ++i) {

		z.g[i] = h*(x.g[i]);
	}

	return z;
}

const GradType sqrt(const GradType& x) {

	GradType z;

	z.f = std::sqrt(x.f);

	const double h = 1.0/(2.0*z.f);

	for (int i=0; i<N; ++i) {

		z.g[i] = h*(x.g[i]);
	}

	return z;
}

}
