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

#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <iosfwd>

namespace gyro {

enum coordinate { X, Y, Z };

template <typename T>
class Vector {

public:

	Vector(T x, T y, T z) { v[X] = x; v[Y] = y; v[Z] = z; }

	const Vector& operator+=(const Vector& x);

	const Vector& operator*=(const Vector& x);

	const Vector& operator*=(double x);

	const Vector& operator/=(double x);

	T operator[] (coordinate i) const { return v[i]; }

	std::ostream& print(std::ostream& os) const  { return os<<v[X]<<'\t'<<v[Y]<<'\t'<<v[Z]; }

private:

	T v[3];
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& x) {

	return x.print(os);
}

template <typename T>
const Vector<T>& Vector<T>::operator+=(const Vector& x) {

	for (int i=0; i<3; ++i) {
		v[i] += x.v[i];
	}

	return *this;
}

template <typename T>
const Vector<T> operator+(const Vector<T>& x, const Vector<T>& y) {

	Vector<T> z(x);

	return z += y;
}


template <typename T>
const Vector<T>& Vector<T>::operator*=(const Vector& x) {

	for (int i=0; i<3; ++i) {
		v[i] *= x.v[i];
	}

	return *this;
}

template <typename T>
const Vector<T> operator*(const Vector<T>& x, const Vector<T>& y) {

	Vector<T> z(x);

	return z *= y;
}

template <typename T>
const Vector<T>& Vector<T>::operator*=(double x) {

	for (int i=0; i<3; ++i) {
		v[i] *= x;
	}

	return *this;
}

template <typename T>
const Vector<T> operator*(const Vector<T>& x, double y) {

	Vector<T> z(x);

	return z*=y;
}

template <typename T>
const Vector<T> operator*(double x, const Vector<T>& y) {

	return y*x;
}

template <typename T>
const Vector<T>& Vector<T>::operator/=(double x) {

	for (int i=0; i<3; ++i) {
		v[i] /= x;
	}

	return *this;
}

template <typename T>
const Vector<T> operator/(const Vector<T>& x, double y) {

	Vector<T> z(x);

	return z/=y;
}

}

#endif
