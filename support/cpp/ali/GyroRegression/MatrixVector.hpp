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

	Vector(const T& x, const T& y, const T& z) { v[X] = x; v[Y] = y; v[Z] = z; }

	Vector(const T x[3]) { v[X] = x[X]; v[Y] = x[Y]; v[Z] = x[Z]; }

	void copy_to(T array[3]) const;

	Vector& operator+=(const Vector& x);

	Vector& operator-=(const Vector& x);

	Vector& operator*=(double x);

	Vector& operator/=(double x);

	template <typename C>
	friend const C operator*(const Vector<C>& x, const Vector<C>& y);

	const T& operator[] (coordinate i) const { return v[i]; }

	std::ostream& print(std::ostream& os) const  { return os<<v[X]<<'\t'<<v[Y]<<'\t'<<v[Z]; }

	template <typename C>
	friend class Matrix;

private:

	T v[3];
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& x) {

	return x.print(os);
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
const Vector<T> operator*(double x, const Vector<T>& y) {

	return y*x;
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
class Matrix {

public:

	Matrix(const T array[9]);

	void copy_to(T array[9]) const;

	Matrix& operator+=(const Matrix& M);

	const Vector<T> operator*(const Vector<T>& v) const;

	const Matrix operator*(const Matrix& M) const;

	std::ostream& print(std::ostream& os) const;

	static const Matrix identity();

private:

	Matrix();

	T m[3][3];
};

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

template <typename T>
const Vector<T> Matrix<T>::operator*(const Vector<T>& x) const {

	Vector<T> z(0.0, 0.0, 0.0);

	for (int i=0; i<3; ++i) {
		for (int j=0; j<3; ++j) {
			z.v[i] += m[i][j]*x.v[j];
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

typedef Vector<double> vector3;
typedef Matrix<double> matrix3;

}

#endif
