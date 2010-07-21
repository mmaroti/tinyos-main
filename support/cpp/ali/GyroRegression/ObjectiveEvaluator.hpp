
#ifndef OBJECTIVEEVALUATOR_HPP_
#define OBJECTIVEEVALUATOR_HPP_

#include <ostream>
#include "InputData.hpp"

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

	const NT* const acc_x;
	const NT* const acc_y;
	const NT* const acc_z;

	const NT* const wx;
	const NT* const wy;
	const NT* const wz;

	const NT dt;
	const int N;

	const NT g_ref;

	std::ostream& log;

	T* MR;
	T* g_err;

	const NT half, one, three;

	bool VERBOSE;

	//==========================================================================

	void set_sum_and_R0() {

		sx = sy = sz = NT(0);

		R_11 = 1.0; R_12 = 0.0; R_13 = 0.0;
		R_21 = 0.0; R_22 = 1.0; R_23 = 0.0;
		R_31 = 0.0; R_32 = 0.0; R_33 = 1.0;
	}

	void compute_G(const int i, const T* const x) {

		C_11 = x[0] + 1.0;
		C_12 = x[1];
		C_13 = x[2];

		C_21 = x[3];
		C_22 = x[4] - 1.0;  // <-- FIXME Fix in calibration
		C_23 = x[5];

		C_31 = x[6];
		C_32 = x[7];
		C_33 = x[8] + 1.0;

		d1 = x[ 9];
		d2 = x[10];
		d3 = x[11];

		T w_x = (C_11*wx[i]+C_12*wy[i]+C_13*wz[i]+d1)*dt;
		T w_y = (C_21*wx[i]+C_22*wy[i]+C_23*wz[i]+d2)*dt;
		T w_z = (C_31*wx[i]+C_32*wy[i]+C_33*wz[i]+d3)*dt;

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

		// TODO Scaling factor?
		sx = sx + a_x;
		sy = sy + a_y;
		sz = sz + a_z;

		if (VERBOSE) {
			log << endl;
			log << "a(i)" << endl;
			log << ax << '\t' << ay << '\t' << az << endl;

			T g_x = M11*a_x+M12*a_y+M13*a_z;
			T g_y = M21*a_x+M22*a_y+M23*a_z;
			T g_z = M31*a_x+M32*a_y+M33*a_z;
			log << endl;
			log << "g(i)" << endl;
			log << g_x << ' ' << g_y << ' ' << g_z << endl;
			//out  << g_x << ' ' << g_y << ' ' << (g_z-g_ref) << endl;
		}

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

	T objective(const T* const x) {
		return -((sx/N)*(sx/N) + (sy/N)*(sy/N) + (sz/N)*(sz/N));
	}

public:

	ObjEval(const input& data, std::ostream& os, bool verbose) :

		acc_x(data.acc_x()),
		acc_y(data.acc_y()),
		acc_z(data.acc_z()),

		wx(data.wx()),
		wy(data.wy()),
		wz(data.wz()),

		dt(data.dt()),

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

			compute_G(i-1, x);

			update_R(); // R(i)=R(i-1)*G(i-1)

			normalize_R();

			sum_Ri_ai(i);

		}

		return objective(x);
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

