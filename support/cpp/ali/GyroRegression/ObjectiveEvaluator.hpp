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

#ifndef OBJECTIVEEVALUATOR_HPP_
#define OBJECTIVEEVALUATOR_HPP_

#include <ostream>
#include "InputData.hpp"
#include "CompileTimeConstants.hpp"

#define OLD_VERSION
#ifdef NEW_VERSION

#include "MatrixVector.hpp"

typedef double NT;

namespace gyro {

template<typename T>
class ObjEval {

private:

	//==========================================================================

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
	Matrix<T> C;
	Vector<T> d;
	Vector<T> s;
	Matrix<T> M;

	//==========================================================================

	void set_sum_R0_C_d(const T* const x) {

		s = Vector<T>(acc_x[0], acc_y[0], acc_z[0]);

		if (MR) {
			s = M*s;
		}

		R = Matrix<T>::identity();

		C = Matrix<T>::identity() + Matrix<T> (x);
		d = Vector<T>(x+9);
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

		T Cx = correction(Rx_new);
		T Cy = correction(Ry_new);
		T Cz = correction(Rz_new);


		R = Matrix<T> (Cx*Rx_new, Cy*Ry_new, Cz*Rz_new);
	}

	void sum_Ri_ai(const int i) {

		const Vector<NT> a_measured(acc_x[i], acc_y[i], acc_z[i]);

		Vector<T> a = R*a_measured;

		if (MR) {

			a = M*a;

			const Matrix<T> RotMat = M*R;
			const int k = 9*i;
			RotMat.copy_to(MR+k);
		}

		s += a;
	}

	T objective() {
		return -((s[X]/N)*(s[X]/N) + (s[Y]/N)*(s[Y]/N) + (s[Z]/N)*(s[Z]/N));
	}

public:

	ObjEval(const Input& data, std::ostream& , bool ) :

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
		d(Vector<T>(0,0,0)),
		s(Vector<T>(0,0,0))
	{

	}

	T f(const T* const x)  {

		set_sum_R0_C_d(x);

		for (int i=1; i<N; ++i) {

			update_R(i); // R(i)=R(i-1)*G(i-1)

			normalize_R();

			sum_Ri_ai(i);

		}

		return objective();
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

#elif defined OLD_VERSION

using std::endl;

typedef double NT;

namespace gyro {

template<typename T>
class ObjEval {

private:

	T R_11, R_12, R_13;
	T R_21, R_22, R_23;
	T R_31, R_32, R_33;

	T Rn_11, Rn_12, Rn_13;
	T Rn_21, Rn_22, Rn_23;
	T Rn_31, Rn_32, Rn_33;

	T G_11, G_12, G_13;
	T G_21, G_22, G_23;
	T G_31, G_32, G_33;

	T C_11, C_12, C_13;
	T C_21, C_22, C_23;
	T C_31, C_32, C_33;

	T  d1,  d2,  d3;

	NT ax, ay, az;
	T  sx, sy, sz;

	//==========================================================================

	NT M11, M12, M13;
	NT M21, M22, M23;
	NT M31, M32, M33;

	//==========================================================================

	const NT* const time_stamp;

	const NT* const acc_x;
	const NT* const acc_y;
	const NT* const acc_z;

	const NT* const wx;
	const NT* const wy;
	const NT* const wz;

	const int N;

	const NT g_ref;

	std::ostream& log;

	T* MR;
	T* g_err;

	const NT half, one, three;

	bool VERBOSE;

	//==========================================================================

	void set_sum_and_R0() {

		R_11 = 1.0; R_12 = 0.0; R_13 = 0.0;
		R_21 = 0.0; R_22 = 1.0; R_23 = 0.0;
		R_31 = 0.0; R_32 = 0.0; R_33 = 1.0;

		ax = acc_x[0]; ay = acc_y[0]; az = acc_z[0];

		sx = ax;
		sy = ay;
		sz = az;

		if (MR) {
			sx = M11*ax+M12*ay+M13*az;
			sy = M21*ax+M22*ay+M23*az;
			sz = M31*ax+M32*ay+M33*az;
		}
	}

	void compute_G(const int i, const T* const x) {

		// FIXME C and d does NOT change with i
		C_11 = x[0] + 1.0;
		C_12 = x[1];
		C_13 = x[2];

		C_21 = x[3];
		C_22 = x[4] + 1.0;  // <-- FIXME Fix in calibration
		C_23 = x[5];

		C_31 = x[6];
		C_32 = x[7];
		C_33 = x[8] + 1.0;

		d1 = x[ 9];
		d2 = x[10];
		d3 = x[11];

		const NT time_step = (time_stamp[i]-time_stamp[i-1])/TICKS_PER_SEC;

		const NT wx_avg = (wx[i]+wx[i-1])/2.0;
		const NT wy_avg = (wy[i]+wy[i-1])/2.0;
		const NT wz_avg = (wz[i]+wz[i-1])/2.0;

		T w_x = (C_11*wx_avg+C_12*wy_avg+C_13*wz_avg+d1)*time_step;
		T w_y = (C_21*wx_avg+C_22*wy_avg+C_23*wz_avg+d2)*time_step;
		T w_z = (C_31*wx_avg+C_32*wy_avg+C_33*wz_avg+d3)*time_step;

		G_11 =  one;
		G_12 = -w_z;
		G_13 =  w_y;

		G_21 =  w_z;
		G_22 =  one;
		G_23 = -w_x;

		G_31 = -w_y;
		G_32 =  w_x;
		G_33 =  one;

		if (VERBOSE) {
			log << endl;
			log << "gyro x, y, z" << endl;
			log << wx[i] << '\t' << wy[i] << '\t' << wz[i] << endl;
			log << endl;
			log << "G_ij" << endl;
			log << G_11 << '\t' << G_12 << '\t' << G_13 << endl;
			log << G_21 << '\t' << G_22 << '\t' << G_23 << endl;
			log << G_31 << '\t' << G_32 << '\t' << G_33 << endl;
		}
	}

	void update_R() {

		Rn_11 = R_11*G_11+R_12*G_21+R_13*G_31;
		Rn_12 = R_11*G_12+R_12*G_22+R_13*G_32;
		Rn_13 = R_11*G_13+R_12*G_23+R_13*G_33;

		Rn_21 = R_21*G_11+R_22*G_21+R_23*G_31;
		Rn_22 = R_21*G_12+R_22*G_22+R_23*G_32;
		Rn_23 = R_21*G_13+R_22*G_23+R_23*G_33;

		Rn_31 = R_31*G_11+R_32*G_21+R_33*G_31;
		Rn_32 = R_31*G_12+R_32*G_22+R_33*G_32;
		Rn_33 = R_31*G_13+R_32*G_23+R_33*G_33;

		//---

		R_11 = Rn_11; R_12 = Rn_12; R_13 = Rn_13;
		R_21 = Rn_21; R_22 = Rn_22; R_23 = Rn_23;
		R_31 = Rn_31; R_32 = Rn_32; R_33 = Rn_33;

		return;
	}

	void normalize_R() {

#ifdef COMPUTE_ERROR
		T err11 = R_13*R_13+R_12*R_12+R_11*R_11 - one;
		T err12 = R_23*R_13+R_22*R_12+R_21*R_11;
		T err13 = R_33*R_13+R_32*R_12+R_31*R_11;

		T err21 = R_13*R_23+R_12*R_22+R_11*R_21;
		T err22 = R_23*R_23+R_22*R_22+R_21*R_21 - one;
		T err23 = R_33*R_23+R_32*R_22+R_31*R_21;

		T err31 = R_13*R_33+R_12*R_32+R_11*R_31;
		T err32 = R_23*R_33+R_22*R_32+R_21*R_31;
		T err33 = R_33*R_33+R_32*R_32+R_31*R_31 - one;
#endif

		T half_error = (R_11*R_21+R_12*R_22+R_13*R_23)*half;

		Rn_11 = R_11-half_error*R_21;
		Rn_12 = R_12-half_error*R_22;
		Rn_13 = R_13-half_error*R_23;

		Rn_21 = R_21-half_error*R_11;
		Rn_22 = R_22-half_error*R_12;
		Rn_23 = R_23-half_error*R_13;

		Rn_31 = Rn_12*Rn_23-Rn_13*Rn_22;
		Rn_32 = Rn_13*Rn_21-Rn_11*Rn_23;
		Rn_33 = Rn_11*Rn_22-Rn_12*Rn_21;

		T C1 = half*(three-(Rn_11*Rn_11+Rn_12*Rn_12+Rn_13*Rn_13));
		T C2 = half*(three-(Rn_21*Rn_21+Rn_22*Rn_22+Rn_23*Rn_23));
		T C3 = half*(three-(Rn_31*Rn_31+Rn_32*Rn_32+Rn_33*Rn_33));

		R_11 = C1*Rn_11;
		R_12 = C1*Rn_12;
		R_13 = C1*Rn_13;

		R_21 = C2*Rn_21;
		R_22 = C2*Rn_22;
		R_23 = C2*Rn_23;

		R_31 = C3*Rn_31;
		R_32 = C3*Rn_32;
		R_33 = C3*Rn_33;

#ifdef COMPUTE_ERROR
		T drr11 = R_13*R_13+R_12*R_12+R_11*R_11 - one;
		T drr12 = R_23*R_13+R_22*R_12+R_21*R_11;
		T drr13 = R_33*R_13+R_32*R_12+R_31*R_11;

		T drr21 = R_13*R_23+R_12*R_22+R_11*R_21;
		T drr22 = R_23*R_23+R_22*R_22+R_21*R_21 - one;
		T drr23 = R_33*R_23+R_32*R_22+R_31*R_21;

		T drr31 = R_13*R_33+R_12*R_32+R_11*R_31;
		T drr32 = R_23*R_33+R_22*R_32+R_21*R_31;
		T drr33 = R_33*R_33+R_32*R_32+R_31*R_31 - one;

		log << scientific ;
		log << err11 << "\t" << drr11 << endl;
		log << err12 << "\t" << drr12 << endl;
		log << err13 << "\t" << drr13 << endl;

		log << err21 << "\t" << drr21 << endl;
		log << err22 << "\t" << drr22 << endl;
		log << err23 << "\t" << drr23 << endl;

		log << err31 << "\t" << drr31 << endl;
		log << err32 << "\t" << drr32 << endl;
		log << err33 << "\t" << drr33 << endl;
#endif

		if (VERBOSE) {
			log << endl;
			log << "R_ij" << endl;
			log << R_11 << '\t' << R_12 << '\t' << R_13 << endl;
			log << R_21 << '\t' << R_22 << '\t' << R_23 << endl;
			log << R_31 << '\t' << R_32 << '\t' << R_33 << endl;
		}

		return;
	}

	void sum_Ri_ai(const int i) {

		ax = acc_x[i]; ay = acc_y[i]; az = acc_z[i];

		T a_x = R_11*ax+R_12*ay+R_13*az;
		T a_y = R_21*ax+R_22*ay+R_23*az;
		T a_z = R_31*ax+R_32*ay+R_33*az;

		if (MR) {
			T g_x = M11*a_x+M12*a_y+M13*a_z;
			T g_y = M21*a_x+M22*a_y+M23*a_z;
			T g_z = M31*a_x+M32*a_y+M33*a_z;
			a_x = g_x;
			a_y = g_y;
			a_z = g_z;
		}
		// TODO Scaling factor?
		sx = sx + a_x;
		sy = sy + a_y;
		sz = sz + a_z;

		if (MR) {

			const int k = 9*i;

			MR[k+0] =  M11*R_11+M12*R_21+M13*R_31;
			MR[k+1] =  M11*R_12+M12*R_22+M13*R_32;
			MR[k+2] =  M11*R_13+M12*R_23+M13*R_33;

			MR[k+3] =  M21*R_11+M22*R_21+M23*R_31;
			MR[k+4] =  M21*R_12+M22*R_22+M23*R_32;
			MR[k+5] =  M21*R_13+M22*R_23+M23*R_33;

			MR[k+6] =  M31*R_11+M32*R_21+M33*R_31;
			MR[k+7] =  M31*R_12+M32*R_22+M33*R_32;
			MR[k+8] =  M31*R_13+M32*R_23+M33*R_33;

			const int m = 3*i;

			g_err[m+0] = MR[k+0]*ax+MR[k+1]*ay+MR[k+2]*az;
			g_err[m+1] = MR[k+3]*ax+MR[k+4]*ay+MR[k+5]*az;
			g_err[m+2] = MR[k+6]*ax+MR[k+7]*ay+MR[k+8]*az - g_ref;
		}

		return;
	}

	T objective() {
		return -((sx/N)*(sx/N) + (sy/N)*(sy/N) + (sz/N)*(sz/N));
	}

public:

	ObjEval(const Input& data, std::ostream& os, bool verbose) :

		time_stamp(data.time_stamp()),

		acc_x(data.acc_x()),
		acc_y(data.acc_y()),
		acc_z(data.acc_z()),

		wx(data.wx()),
		wy(data.wy()),
		wz(data.wz()),

		N(data.N()),

		g_ref(data.g_ref()),
		log(os),
		MR(0),
		g_err(0),
		half(NT(0.5)),
		one(NT(1)),
		three(NT(3))
	{
		VERBOSE = verbose;
	}

	T f(const T* const x)  {

		set_sum_and_R0();

		for (int i=1; i<N; ++i) {

			if (VERBOSE) {
				// log << std::setprecision(4); TODO on the caller's side
				// log << std::fixed;
				log << "-----------------------------------------------------" << endl;
				log << "Step #" << i << endl;
			}

			compute_G(i, x);

			update_R(); // R(i)=R(i-1)*G(i-1)

			normalize_R();

			sum_Ri_ai(i);

		}

		return objective();
	}

	const T s_x() const { return sx/N; }

	const T s_y() const { return sy/N; }

	const T s_z() const { return sz/N; }

	void set_verbose() { VERBOSE = true; }

	void unset_verbose() { VERBOSE = false; }

	void set_M(double* R, double* g_error) {

		M11 = R[0]; M12 = R[1]; M13 = R[2];
		M21 = R[3]; M22 = R[4]; M23 = R[5];
		M31 = R[6]; M32 = R[7]; M33 = R[8];

		MR = R;
		g_err = g_error;
	}

};

}

#endif

#endif

