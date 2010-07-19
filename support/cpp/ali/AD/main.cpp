#include "HessType.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace {

const int N_VARS = 12;
const int N_CONS = 0;

double M11, M12, M13;
double M21, M22, M23;
double M31, M32, M33;

ofstream out;

double solution[N_VARS];

}

typedef double NT;

namespace input {

NT* acc_x; NT* acc_y; NT* acc_z;
NT* wx; NT* wy; NT* wz;
int N;

NT dt, g_ref;

}

void init(const char* const filename) {

	using namespace input;

	dt    = NT(10.0/2048.0);
	g_ref = NT(-9.81);

	//--------------------------------------------------------------------------

	std::ifstream in(filename);

	if (!in.good()) {
		cerr << endl << "Failed to open input file!" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Reading input file: " << filename << endl;

	in >> N;

	if (N<1) {
		cerr << endl << "Invalid lenght!" << endl;
		exit(EXIT_FAILURE);
	}

	// FIXME Resources never released
	//----------------------------------------------------------------------

	wx = new NT[N];
	wy = new NT[N];
	wz = new NT[N];

	acc_x = new NT[N];
	acc_y = new NT[N];
	acc_z = new NT[N];

	//---------------------------------------------------

	double dummy(0.0);

	for (int i=0; i<N; ++i) {

		in >> acc_x[i];
		in >> acc_y[i];
		in >> acc_z[i];

		in >> dummy; in >> dummy; in >> dummy; in >> dummy;


		in >> wx[i];
		in >> wy[i];
		in >> wz[i];

		if (!in.good()) {
			cerr << endl << "Problems on reading from input" << endl;
			exit(EXIT_FAILURE);
		}
	}

	// -------------------------------------------------------------------------

	string fname(filename);

	fname.append("_gerr");

	out.open(fname.c_str());
	out << setprecision(16);
	out << scientific;
}

template<typename T>
class glob {

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

	NT half, one, three, dt;

	NT* wx; NT* wy; NT* wz;

	NT* acc_x; NT* acc_y; NT* acc_z;

	int N;

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
			cout << endl;
			cout << "gyro x, y, z" << endl;
			cout << wx[i] << '\t' << wy[i] << '\t' << wz[i] << endl;
			cout << endl;
			cout << "G_ij" << endl;
			cout << G_11 << '\t' << G_12 << '\t' << G_13 << endl;
			cout << G_21 << '\t' << G_22 << '\t' << G_23 << endl;
			cout << G_31 << '\t' << G_32 << '\t' << G_33 << endl;
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

		cout << scientific ;
		cout << err11 << "\t" << drr11 << endl;
		cout << err12 << "\t" << drr12 << endl;
		cout << err13 << "\t" << drr13 << endl;

		cout << err21 << "\t" << drr21 << endl;
		cout << err22 << "\t" << drr22 << endl;
		cout << err23 << "\t" << drr23 << endl;

		cout << err31 << "\t" << drr31 << endl;
		cout << err32 << "\t" << drr32 << endl;
		cout << err33 << "\t" << drr33 << endl;
#endif

		if (VERBOSE) {
			cout << endl;
			cout << "R_ij" << endl;
			cout << R_11 << '\t' << R_12 << '\t' << R_13 << endl;
			cout << R_21 << '\t' << R_22 << '\t' << R_23 << endl;
			cout << R_31 << '\t' << R_32 << '\t' << R_33 << endl;
		}

		return;
	}

	void sum_Ri_ai(const int i) {

		ax = acc_x[i]; ay = acc_y[i]; az = acc_z[i];

		T a_x = R_11*ax+R_12*ay+R_13*az;
		T a_y = R_21*ax+R_22*ay+R_23*az;
		T a_z = R_31*ax+R_32*ay+R_33*az;

		if (VERBOSE) {
			cout << endl;
			cout << "a(i)" << endl;
			cout << ax << '\t' << ay << '\t' << az << endl;

			T g_x = M11*a_x+M12*a_y+M13*a_z;
			T g_y = M21*a_x+M22*a_y+M23*a_z;
			T g_z = M31*a_x+M32*a_y+M33*a_z;
			cout << endl;
			cout << "g(i)" << endl;
			cout << g_x << ' ' << g_y << ' ' << g_z << endl;
			out  << g_x << ' ' << g_y << ' ' << (g_z-input::g_ref) << endl;
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

	glob(	double* acc_x,
			double* acc_y,
			double* acc_z,
			double* wx,
			double* wy,
			double* wz,
			int N,
			double dt)
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
	}

	T f(const T* const x)  {

		set_sum_and_R0();

		for (int i=1; i<N; ++i) {

			if (VERBOSE) {
				cout << setprecision(4);
				cout << fixed ;
				cout << "-----------------------------------------------------" << endl;
				cout << "Step #" << i << endl;
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

int main() {

	init("manual");

	using namespace input;
	static glob<HessType<N_VARS> > gv(acc_x, acc_y, acc_z, wx, wy, wz, N, dt);

	HessType<N_VARS> x[N_VARS];
	for (int i=0; i<N_VARS; ++i)
		x[i] = 0.001;

	cout << "f(0): " << gv.f(x) << endl;

	time_t start, end;

	const int rep = 30;

	time (&start);

	for (int i=0; i<rep; ++i) {
		gv.f(x);
	}
	time (&end);

	const double dif = difftime (end, start);

	cout << "Computed " << rep << " times, took " << dif << " seconds" << endl;

/*
	const int N_VARS = 2;
	double v[] = { 3.0, 5.0 };

	HessType<N_VARS> a;
	HessType<N_VARS> b(1.0);
	HessType<N_VARS> c(a);
	a = c;
	const double three = 3.0;
	c = three;

	HessType<N_VARS> vars[N_VARS];
	init_vars(vars, v);

	c =  -c;
	b = a+c;
	c = b-a;
	c = a*b;
	c = a/b;

	std::cout << "c: " << c << std::endl;
*/
	return 0;
}

