/* Copyright (c) 2011, University of Szeged
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

#include <cmath>
#include <ostream>
#include "MatrixVector.cpp"

namespace gyro {

template <>
const double Vector<double>::length() const {

	return std::sqrt(std::pow(v[X],2)+std::pow(v[Y],2)+std::pow(v[Z],2));
}

template <>
void Vector<double>::enforce_range_minus_pi_plus_pi() {

	const double PI(3.14159265358979323846);
	const double PI_TIMES_2(6.28318530717958647693);

	for (int i=0; i<3; ++i) {

		while (v[i] < -PI) {
			v[i] += PI_TIMES_2;
		}

		while (v[i] >  PI) {
			v[i] -= PI_TIMES_2;
		}
	}
}

template class Vector<double>;
template class Matrix<double>;

template const vector3 operator+(const vector3& x, const vector3& y);
template const vector3 operator-(const vector3& x, const vector3& y);
template const vector3 operator*(const vector3& x, double y);
template const vector3 operator/(const vector3& x, double y);
template const matrix3 operator+(const matrix3& A, const matrix3& B);

template std::ostream& operator<<(std::ostream& os, const vector3& x);
template const double  operator*(const vector3& x, const vector3& y);
template const vector3 operator*(const double& c, const vector3& x);
template const vector3 cross_product(const vector3& x, const vector3& y);

template const vector3 operator*(const matrix3& M, const vector3& v);

}

