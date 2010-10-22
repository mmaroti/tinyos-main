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

#ifndef OBJECTIVE_HPP_
#define OBJECTIVE_HPP_

#include <ostream>
#include "InputData.hpp"
#include "MatrixVector.hpp"

typedef double NT;

namespace gyro {

template<typename T>
class Objective {

private:

	const NT* const time_stamp;

	const NT* const acc_x;
	const NT* const acc_y;
	const NT* const acc_z;

	const NT* const wx;
	const NT* const wy;
	const NT* const wz;

	const int N;

	const NT g_ref;

	T* MR;

	Matrix<T> R;
	Matrix<T> A;
	Vector<T> b;
	Matrix<T> C;
	Vector<T> d;
	Vector<T> s;
	Matrix<T> M;

	Vector<T> a_sum;
	T error;

	void set_R0_A_b_C_d_sum(const T* const x) {

		R = Matrix<T>::identity();

		//A = Matrix<T>::identity() + Matrix<T> (x);
		A = Matrix<T>::identity();
		b = Vector<T>(x+9);
		//b = Vector<T> (0,0,0);

		C = Matrix<T>::identity() + Matrix<T> (x+12);
		d = Vector<T>(x+21);

		s = A*acceleration(0)+b;

		if (MR) {
			s = M*s;
		}

		error = (s - a_sum).length();
	}

	const Vector<NT> acceleration(int i) const {

		return Vector<NT> (acc_x[i], acc_y[i], acc_z[i]);
	}

	const Vector<NT> angular_rate(int i) const {

		return Vector<NT> (wx[i], wy[i], wz[i]);
	}

	const NT time_step(int i) const {

		return (time_stamp[i]-time_stamp[i-1])/TICKS_PER_SEC;
	}

	void update_R(const int i) {

		const Vector<NT> angular_rate_avg = (angular_rate(i-1)+angular_rate(i))/2;

		Vector<T> angle = (C*angular_rate_avg+d)*time_step(i);

		T tmp[] = {    T(1.0), -angle[Z],  angle[Y],
				     angle[Z],    T(1.0), -angle[X],
				    -angle[Y],  angle[X],    T(1.0)
		};

		const Matrix<T> G(tmp);

		R = R*G;
	}

	const T correction(const Vector<T>& v) const {

		return (3-v*v)/2;
	}

	void normalize_R() {

		const Vector<T> Rx = R[X];
		const Vector<T> Ry = R[Y];

		T half_error = (Rx*Ry)/2;

		const Vector<T> Rx_new = Rx-half_error*Ry;
		const Vector<T> Ry_new = Ry-half_error*Rx;
		const Vector<T> Rz_new = cross_product(Rx_new, Ry_new);

		T Cx = correction(Rx_new); // TODO Write *= with arg T, not double
		T Cy = correction(Ry_new);
		T Cz = correction(Rz_new);

		R = Matrix<T> (Cx*Rx_new, Cy*Ry_new, Cz*Rz_new);
	}

	void sum_Ri_ai(const int i) {

		const Vector<T> accel = A*acceleration(i)+b;

		Vector<T> a = R*accel;

		if (MR) {

			a = M*a;

			const Matrix<T> RotMat = M*R;
			const int k = 9*i;
			RotMat.copy_to(MR+k);
		}

		s += a;

		error += (a - a_sum).length();
	}

	T objective() {
		return -((s[X]/N)*(s[X]/N) + (s[Y]/N)*(s[Y]/N) + (s[Z]/N)*(s[Z]/N));
	}

	void evaluate(const T* const x) {

		set_R0_A_b_C_d_sum(x);

		for (int i=1; i<N; ++i) {

			update_R(i); // R(i)=R(i-1)*G(i-1)

			normalize_R();

			sum_Ri_ai(i);
		}
	}

public:

	Objective(const Input& data, std::ostream& , bool ) :

		time_stamp(data.time_stamp()),

		acc_x(data.acc_x()),
		acc_y(data.acc_y()),
		acc_z(data.acc_z()),

		wx(data.wx()),
		wy(data.wy()),
		wz(data.wz()),

		N(data.N()),

		g_ref(data.g_ref()),
		MR(0),
		b(Vector<T>(0,0,0)),
		d(Vector<T>(0,0,0)),
		s(Vector<T>(0,0,0)),
		a_sum(Vector<T>(0,0,0))
	{

	}

	T f(const T* const x)  {

		a_sum = Vector<T> (0,0,0);

		evaluate(x);

		a_sum = s/N;

		evaluate(x);

		return error;
	}

	const T s_x() const { return s[X]/N; }

	const T s_y() const { return s[Y]/N; }

	const T s_z() const { return s[Z]/N; }

	void set_M(double* R, double* g_error) {

		M = Matrix<NT> (R);
		MR = R;
	}

};

}

#endif
