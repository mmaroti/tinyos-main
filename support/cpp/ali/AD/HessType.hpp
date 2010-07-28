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

#ifndef HESSTYPE_HPP_
#define HESSTYPE_HPP_

#include <ostream>

template <int N>
class HessType {

public:

	HessType() { }

	HessType(double constant);

	HessType(const HessType& other);

	HessType& operator=(const HessType& rhs);

	HessType& operator=(double rhs);

	template <int N_VAR>
	friend void init_vars(HessType<N_VAR> var[N_VAR], const double* const x);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const double y);

	std::ostream& print(std::ostream& os) const;

private:

	void copy(const HessType& other);
	void init(double value);

	double f;
	double g[N];
	double h[N][N];
};

template <int N>
void HessType<N>::init(double value) {
	f = value;
	for (int i=0; i<N; ++i) {
		g[i] = 0.0;
		for (int j=0; j<=i; ++j) {
			h[i][j] = 0.0;
		}
	}
}

template <int N>
void HessType<N>::copy(const HessType& other) {
	f = other.f;
	for (int i=0; i<N; ++i) {
		g[i] = other.g[i];
		for (int j=0; j<=i; ++j) {
			h[i][j] = other.h[i][j];
		}
	}
}

template <int N>
HessType<N>::HessType(double constant) {
	init(constant);
}

template <int N>
HessType<N>::HessType(const HessType& other) {
	copy(other);
}

template <int N>
HessType<N>& HessType<N>::operator=(const HessType<N>& rhs) {
	copy(rhs);
	return *this;
}

template <int N>
HessType<N>& HessType<N>::operator=(double rhs) {
	init(rhs);
	return *this;
}

template <int N_VAR>
void init_vars(HessType<N_VAR> var[N_VAR], const double* x) {

	for (int i=0; i<N_VAR; ++i) {
		var[i] = x[i];
		var[i].g[i] = 1.0;
	}
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x) {

	HessType<N_VAR> z;

	z.f = -x.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = -x.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = -x.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f + y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] + y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x.h[i][j] + y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z(y);

	z.f += x;

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const double y) {

	return y+x;
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f - y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] - y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x.h[i][j] - y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator-(const double x, const HessType<N_VAR>& y) {

	return x+(-y);
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const double y) {

	return x+(-y);
}

template <int N_VAR>
const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = y.f*x.g[i] + x.f*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = y.f*x.h[i][j]+x.g[i]*y.g[j]+y.g[i]*x.g[j]+x.f*y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator*(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x*y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const double y) {

	return y*x;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = (x.g[i] - z.f*y.g[i]) / y.f;
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = (x.h[i][j]-z.g[i]*y.g[j]-y.g[i]*z.g[j]-z.f*y.h[i][j])/y.f;
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x/y.f;
	const double p = -z.f/y.f;
	const double q = (-2.0*p)/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = p*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = p*y.h[i][j] + q*y.g[i]*y.g[j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const double y) {

	return x*(1.0/y);
}

template <int N_VAR>
std::ostream& HessType<N_VAR>::print(std::ostream& os) const {
	os << this->f << std::flush;
	return os;
}

template <int N_VAR>
std::ostream& operator<<(std::ostream& os, const HessType<N_VAR>& x) {
	return x.print(os);
}

#endif
