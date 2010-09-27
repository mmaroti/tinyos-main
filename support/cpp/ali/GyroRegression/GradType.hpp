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

#ifndef GRADTYPE_HPP_
#define GRADTYPE_HPP_

#include <iosfwd>

namespace gyro {

template <int N>
class GradType {

public:

	GradType() { }

	GradType(double constant) { init(constant); }

	GradType(const GradType& other) { copy(other); }

	GradType& operator=(const GradType& rhs) { copy(rhs); return *this; }

	GradType& operator=(double rhs) { init(rhs); return *this; }

	template <int N_VAR>
	friend void init_vars(GradType<N_VAR> var[N_VAR], const double* const x);

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x);

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const double x, const GradType<N_VAR>& y)
	{
		GradType<N_VAR> z(y); z.f += x; return z;
	}

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const double y) { return y+x; }

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const double x, const GradType<N_VAR>& y) { return x+(-y); }

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const double y) { return x+(-y); }

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const double x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const double y) { return y*x; }

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const double x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const double y) { return x*(1.0/y); }

	double value() const { return f; }

	const double* gradient() const { return g; }

	int size() const { return N; }

	std::ostream& print(std::ostream& os) const { os << f; return os; }

private:

	void copy(const GradType& other);
	void init(double value);

	double f;
	double g[N];
};

template <int N_VAR>
std::ostream& operator<<(std::ostream& os, const GradType<N_VAR>& x) {
	return x.print(os);
}

template <int N>
void GradType<N>::init(double value) {
	f = value;
	for (int i=0; i<N; ++i) {
		g[i] = 0.0;
	}
}

template <int N>
void GradType<N>::copy(const GradType& other) {
	f = other.f;
	for (int i=0; i<N; ++i) {
		g[i] = other.g[i];
	}
}

template <int N_VAR>
void init_vars(GradType<N_VAR> var[N_VAR], const double* x) {

	for (int i=0; i<N_VAR; ++i) {
		var[i] = x[i];
		var[i].g[i] = 1.0;
	}
}

template <int N_VAR>
const GradType<N_VAR> operator-(const GradType<N_VAR>& x) {

	GradType<N_VAR> z;

	z.f = -x.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = -x.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f + y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] + y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f - y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] - y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = y.f*x.g[i] + x.f*y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator*(const double x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x*y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = (x.g[i] - z.f*y.g[i]) / y.f;
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator/(const double x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x/y.f;
	const double p = -z.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = p*y.g[i];
	}

	return z;
}

}

#endif
