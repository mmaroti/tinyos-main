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

#ifndef MATRIXVECTOR_HPP_
#define MATRIXVECTOR_HPP_

#include <iosfwd>

namespace gyro {

enum coordinate { X, Y, Z };

template <typename T>
class Vector {

public:

	Vector() { }

	Vector(T x, T y, T z) { v[X] = x; v[Y] = y; v[Z] = z; }

	const Vector& operator+=(const Vector& x);

	const Vector& operator*=(double x);

	const Vector& operator/=(double x);

	template <typename C>
	friend const C operator*(const Vector<C>& x, const Vector<C>& y);

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
const T operator*(const Vector<T>& x, const Vector<T>& y) {

	return x.v[X]*y.v[X]+x.v[Y]*y.v[Y]+x.v[Z]*y.v[Z];
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

template <typename T>
class Matrix {

public:

	Matrix(const T mat[9]);

	template <typename C>
	friend const Vector<C> operator*(const Matrix<C>& M, const Vector<C>& v);

	template <typename C>
	friend const Matrix<C> operator*(const Matrix<C>& M, const Matrix<C>& v);

	std::ostream& print(std::ostream& os) const;

private:

	Vector<T> row[3];
};

template <typename T>
Matrix<T>::Matrix(const T mat[9]) {

	row[0] = Vector<T> (mat[0], mat[1], mat[2]);
	row[1] = Vector<T> (mat[3], mat[4], mat[5]);
	row[2] = Vector<T> (mat[6], mat[7], mat[8]);
}

template <typename T>
std::ostream& Matrix<T>::print(std::ostream& os) const {

	os << '\n';

	for (int i=0; i<3; ++i) {
		os << row[i] << '\n';
	}

	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& x) {

	return x.print(os);
}

template <typename C>
const Vector<C> operator*(const Matrix<C>& M, const Vector<C>& v) {

	return Vector<C> (M.row[X]*v, M.row[Y]*v, M.row[Z]*v);
}

}

#endif
