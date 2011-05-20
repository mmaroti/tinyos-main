/* Copyright (c) 2010, 2011 University of Szeged
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
*      Author: Ali Baharev
*/

#include <ostream>
#include "MatrixVector.hpp"

namespace gyro {

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& x) {

	return x.print(os);
}

template <typename T>
const T Vector<T>::length() const {

	return sqrt(pow(v[X],2)+pow(v[Y],2)+pow(v[Z],2));
}

template <typename T>
void Vector<T>::copy_to(T array[3]) const {

	for (int i=0; i<3; ++i) {
		array[i] = v[i];
	}
}

template <typename T>
Vector<T>& Vector<T>::operator+=(const Vector& x) {

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
Vector<T>& Vector<T>::operator-=(const Vector& x) {

	for (int i=0; i<3; ++i) {
		v[i] -= x.v[i];
	}

	return *this;
}

template <typename T>
const Vector<T> operator-(const Vector<T>& x, const Vector<T>& y) {

	Vector<T> z(x);

	return z -= y;
}

template <typename T>
const T operator*(const Vector<T>& x, const Vector<T>& y) {

	return x.v[X]*y.v[X]+x.v[Y]*y.v[Y]+x.v[Z]*y.v[Z];
}

template <typename T>
const Vector<T> operator*(const T& c, const Vector<T>& y) {

	return Vector<T> (c*y.v[X], c*y.v[Y], c*y.v[Z]);
}

template <typename T>
Vector<T>& Vector<T>::operator*=(double x) {

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
Vector<T>& Vector<T>::operator/=(double x) {

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
const Vector<T> cross_product(const Vector<T>& x, const Vector<T>& y) {
	const T* const a = x.v;
	const T* const b = y.v;

	return Vector<T>(a[Y]*b[Z]-a[Z]*b[Y], a[Z]*b[X]-a[X]*b[Z], a[X]*b[Y]-a[Y]*b[X]);
}

template <typename T>
Matrix<T>::Matrix(const T array[9]) {

	for (int i=0, k=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			m[i][j] = array[k++];
		}
	}
}

template <typename T>
void Matrix<T>::copy_to(T array[9]) const {

	for (int i=0, k=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			array[k++] = m[i][j];
		}
	}
}

template <typename T>
Matrix<T>::Matrix() {

	for (int i=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			m[i][j] = T(0.0);
		}
	}
}

template <typename T>
Matrix<T>::Matrix(	const T& x1, const T& x2, const T& x3,
					const T& x4, const T& x5, const T& x6,
					const T& x7, const T& x8, const T& x9)
{
	m[X][X] = x1; m[X][Y] = x2; m[X][Z] = x3;
	m[Y][X] = x4; m[Y][Y] = x5; m[Y][Z] = x6;
	m[Z][X] = x7; m[Z][Y] = x8; m[Z][Z] = x9;
}

template <typename T>
Matrix<T>::Matrix(const Vector<T>& row_x, const Vector<T>& row_y, const Vector<T>& row_z) {

	row_x.copy_to(m[X]);
	row_y.copy_to(m[Y]);
	row_z.copy_to(m[Z]);
}

template <typename T>
const Matrix<T> Matrix<T>::identity() {

	Matrix I;

	for (int i=0; i<3; ++i) {
		I.m[i][i] = T(1.0);
	}

	return I;
}

template <typename T>
std::ostream& Matrix<T>::print(std::ostream& os) const {

	os << '\n';

	for (int i=0; i<3; ++i) {
		os << m[i][0] << '\t' << m[i][1] << '\t' << m[i][2] << '\n';
	}

	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& x) {

	return x.print(os);
}

template <typename U, typename V>
const Vector<U> operator*(const Matrix<U>& A, const Vector<V>& x) {

	Vector<U> z(0.0, 0.0, 0.0);

	for (int i=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			z.v[i] += A.m[i][j]*x.v[j];
		}
	}

	return z;
}

template <typename T>
const Matrix<T> Matrix<T>::operator*(const Matrix<T>& M) const {

	Matrix<T> z;

	for (int i=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			for (int k=0; k<3; ++k) {
				z.m[i][j] += m[i][k]*M.m[k][j];
			}
		}
	}

	return z;
}

template <typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& M) {

	for (int i=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			m[i][j] += M.m[i][j];
		}
	}

	return *this;
}

template <typename T>
const Matrix<T> operator+(const Matrix<T>& A, const Matrix<T>& B) {

	Matrix<T> C(A);

	return C+=B;
}

template <typename T>
const Vector<T> Matrix<T>::operator[] (coordinate i) const {

	return Vector<T> (m[i][X], m[i][Y], m[i][Z]);
}

}

