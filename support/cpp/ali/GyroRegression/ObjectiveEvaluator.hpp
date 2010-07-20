
#ifndef OBJECTIVEEVALUATOR_HPP_
#define OBJECTIVEEVALUATOR_HPP_

#include <iostream>
#include <iomanip>

using std::endl;

typedef double NT;

template<typename T>
class ObjEval {

private:

	std::ostream& out;
	std::ostream& log;

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

	NT half, one, three, dt;

	NT* wx; NT* wy; NT* wz;

	NT* acc_x; NT* acc_y; NT* acc_z;

	int N;

	NT g_ref;

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
		C_22 = x[4] - 1.0;  // <-- FIXME
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
			out  << g_x << ' ' << g_y << ' ' << (g_z-g_ref) << endl;
		}

		// TODO Scaling factor?
		sx = sx + a_x;
		sy = sy + a_y;
		sz = sz + a_z;
		return;
	}

	T objective(const T* const x) {
		return -((sx/N)*(sx/N) + (sy/N)*(sy/N) + (sz/N)*(sz/N));
	}

public:

	ObjEval(std::ostream& os,
			double* acc_x,
			double* acc_y,
			double* acc_z,
			double* wx,
			double* wy,
			double* wz,
			int N,
			double g_reference,
			double dt) : out(os), log(os)
	{

		half  = NT(0.5);
		one   = NT(1);
		three = NT(3);
		VERBOSE = false;

		this->acc_x = acc_x;
		this->acc_y = acc_y;
		this->acc_z = acc_z;

		this->wx = wx;
		this->wy = wy;
		this->wz = wz;

		this->dt = dt;

		this->N = N;

		g_ref = g_reference;
	}

	T f(const T* const x)  {

		set_sum_and_R0();

		for (int i=1; i<N; ++i) {

			if (VERBOSE) {
				log << std::setprecision(4);
				log << std::fixed;
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

};


#endif

