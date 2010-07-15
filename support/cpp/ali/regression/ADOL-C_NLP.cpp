#include <fstream>
#include <iomanip>
#include "ADOL-C_NLP.hpp"

using namespace Ipopt;

namespace {

const int N_VARS = 15;
const int N_CONS = 1;

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

NT ax0, ay0, az0; // FIXME Hideous...

}

void estimate_a0() {

	using namespace input;

	ax0 = acc_x[0];
	ay0 = acc_y[0];
	az0 = acc_z[0];

	NT length = std::sqrt(ax0*ax0+ay0*ay0+az0*az0);

	NT corr = std::fabs(g_ref)/length;

	ax0 *= corr;
	ay0 *= corr;
	az0 *= corr;
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

	estimate_a0();

	// -------------------------------------------------------------------------

	string fname(filename);

	fname.append(".gerr");

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

	const bool VERBOSE;

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
			out << g_x << ' ' << g_y << ' ' << (g_z-input::g_ref) << endl;
		}

		// TODO Scaling factor?
		sx = sx + a_x;
		sy = sy + a_y;
		sz = sz + a_z;
		return;
	}

	T objective(const T* const x) {
		return (sx/N-x[12])*(sx/N-x[12]) + (sy/N-x[13])*(sy/N-x[13]) + (sz/N-x[14])*(sz/N-x[14]);
	}

public:

	glob(	double* acc_x,
			double* acc_y,
			double* acc_z,
			double* wx,
			double* wy,
			double* wz,
			int N,
			double dt,
			bool verbose = false)
	: VERBOSE(verbose)
	{

		half  = NT(0.5);
		one   = NT(1);
		three = NT(3);

		this->acc_x = acc_x;
		this->acc_y = acc_y;
		this->acc_z = acc_z;

		this->wx = wx;
		this->wy = wy;
		this->wz = wz;

		this->dt    = dt;

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

};

void dump(bool use_hardcoded) {

	using std::sqrt;
	using std::pow;

	using namespace input;
	glob<double> obj(acc_x, acc_y, acc_z, wx, wy, wz, N, dt, true);

	// manual2
	double y[] = {
			0.00847338,
			-0.0189621,
			0.00120593,
			0.00561178,
			-0.0337569,
			-0.00391856,
			0.00880578,
			0.003009,
			-0.0290832,
			-0.0657448,
			-0.273428,
			0.183664,
			4.80704,
			-6.42853,
			-5.63937
	};

	/*
	// manual
	double y[] = {
			0.00675085,
			-0.0200092,
			0.00815529,
			0.00348722,
			-0.03278,
			-0.00216696,
			-0.00196037,
			0.0210999,
			-0.0313881,
			-0.0666635,
			-0.272655,
			0.192581,
			-0.580514,
			0.365056,
			-9.786
	};
	 */

	//==========================================================================


	double* x = y;

	if (!use_hardcoded) {
		x = solution;
	}

	const double ax = x[12];
	const double ay = x[13];
	const double az = x[14];

	const double axy = sqrt(pow(ax, 2)+pow(ay, 2));

	// FIXME axy == 0 ?

	const double ux = -ay/axy;
	const double uy =  ax/axy;
	//           uz =  0.0;

	const double ux2 = pow(ux, 2);
	const double uy2 = pow(uy, 2);
	const double uxy = ux*uy;

	const double a = sqrt(pow(ax, 2)+pow(ay, 2)+pow(az, 2));

	const double c = -az/a;

	const double s = sqrt(1.0-pow(c, 2));

	M11 = ux2+uy2*c;
	M12 = uxy*(1.0-c);
	M13 = uy*s;

	M21 = M12;
	M22 = uy2+ux2*c;
	M23 =-ux*s;

	M31 =-M13;
	M32 =-M23;
	M33 = c;

	double z = obj.f(x);

	cout << "===========================================================" << endl;
	cout << "Objective: " << z << endl;
	return;
}

void dump_Ri_using_hardcoded() {
	dump(true);
}

void dump_Ri() {
	dump(false);
}

template<class T> bool  MyADOLC_NLP::eval_obj(Index n, const T *x, T& obj_value)
{
	using namespace input;
	static glob<T> gv(acc_x, acc_y, acc_z, wx, wy, wz, N, dt);

	obj_value = gv.f(x);

	return true;
}

template<class T> bool  MyADOLC_NLP::eval_constraints(Index n, const T *x, Index m, T* g)
{
	// FIXME Hideous
	using namespace input;
	g[0] = x[12]*x[12] + x[13]*x[13] + x[14]*x[14] - g_ref*g_ref;
	return true;
}

bool MyADOLC_NLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
		Index m, Number* g_l, Number* g_u)
{

	for (Index i=0; i<n; i++) {
		x_l[i] = -1.0;
		x_u[i] =  1.0;
	}

	for (int i=12; i<=14; ++i) {
		x_l[i] = -10;
		x_u[i] =  10;
	}

	// Set the bounds for the constraints
	for (Index i=0; i<m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}

	return true;
}

bool MyADOLC_NLP::get_starting_point(Index n, bool init_x, Number* x,
		bool init_z, Number* z_L, Number* z_U,
		Index m, bool init_lambda,
		Number* lambda)
{
	assert(init_x == true);
	assert(init_z == false);
	assert(init_lambda == false);

	// set the starting point
	for (Index i=0; i<n; i++)
		x[i] = 0.0;

	// FIXME Hideous
	using namespace input;

	x[12] =  ax0;
	x[13] =  ay0;
	x[14] =  az0;

	return true;
}

//==============================================================================
//==============================================================================

MyADOLC_NLP::MyADOLC_NLP() {}

MyADOLC_NLP::~MyADOLC_NLP(){}

bool MyADOLC_NLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	n = N_VARS;

	m = N_CONS;

	// in this example the jacobian is dense. Hence, it contains n*m nonzeros
	nnz_jac_g = n*m;

	// the hessian is also dense and has n*n total nonzeros, but we
	// only need the lower left corner (since it is symmetric)
	nnz_h_lag = n*(n-1)/2+n;

	generate_tapes(n, m);

	// use the C style indexing (0-based)
	index_style = C_STYLE;

	return true;
}

//*************************************************************************
//
//
//         Nothing has to be changed below this point !!
//
//
//*************************************************************************


bool MyADOLC_NLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
	eval_obj(n,x,obj_value);

	return true;
}

bool MyADOLC_NLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{

	gradient(tag_f,n,x,grad_f);

	return true;
}

bool MyADOLC_NLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{

	eval_constraints(n,x,m,g);

	return true;
}

bool MyADOLC_NLP::eval_jac_g(Index n, const Number* x, bool new_x,
		Index m, Index nele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	if (values == NULL) {
		// return the structure of the jacobian,
		// assuming that the Jacobian is dense

		Index idx = 0;
		for(Index i=0; i<m; i++)
			for(Index j=0; j<n; j++)
			{
				iRow[idx] = i;
				jCol[idx++] = j;
			}
	}
	else {
		// return the values of the jacobian of the constraints

		jacobian(tag_g,m,n,x,Jac);

		Index idx = 0;
		for(Index i=0; i<m; i++)
			for(Index j=0; j<n; j++)
				values[idx++] = Jac[i][j];

	}

	return true;
}

bool MyADOLC_NLP::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda,
		bool new_lambda, Index nele_hess, Index* iRow,
		Index* jCol, Number* values)
{
	if (values == NULL) {
		// return the structure. This is a symmetric matrix, fill the lower left
		// triangle only.

		// the hessian for this problem is actually dense
		Index idx=0;
		for (Index row = 0; row < n; row++) {
			for (Index col = 0; col <= row; col++) {
				iRow[idx] = row;
				jCol[idx] = col;
				idx++;
			}
		}

		assert(idx == nele_hess);
	}
	else {
		// return the values. This is a symmetric matrix, fill the lower left
		// triangle only

		for(Index i = 0; i<n ; i++)
			x_lam[i] = x[i];
		for(Index i = 0; i<m ; i++)
			x_lam[n+i] = lambda[i];
		x_lam[n+m] = obj_factor;

		hessian(tag_L,n+m+1,x_lam,Hess);

		Index idx = 0;

		for(Index i = 0; i<n ; i++)
		{
			for(Index j = 0; j<=i ; j++)
			{
				values[idx++] = Hess[i][j];
			}
		}
	}

	return true;
}

void MyADOLC_NLP::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U,
		Index m, const Number* g, const Number* lambda,
		Number obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq)
{


	cout << endl << "Solution vector:" << endl;
	for (int i=0; i<n; ++i) {
		cout << i << '\t' << x[i] << endl;
	}
	cout << endl;

	for (int i=0; i<n; ++i) {
		solution[i] = x[i];
	}

	// Memory deallocation for ADOL-C variables

	delete[] x_lam;

	for(Index i=0;i<m;i++)
		delete[] Jac[i];
	delete[] Jac;

	for(Index i=0;i<n+m+1;i++)
		delete[] Hess[i];
	delete[] Hess;
}


//***************    ADOL-C part ***********************************

void MyADOLC_NLP::generate_tapes(Index n, Index m)
{
	Number *xp    = new double[n];
	Number *lamp  = new double[m];
	Number *zl    = new double[m];
	Number *zu    = new double[m];

	adouble *xa   = new adouble[n];
	adouble *g    = new adouble[m];
	adouble *lam  = new adouble[m];
	adouble sig;
	adouble obj_value;

	double dummy;

	Jac = new double*[m];
	for(Index i=0;i<m;i++)
		Jac[i] = new double[n];

	x_lam   = new double[n+m+1];

	Hess = new double*[n+m+1];
	for(Index i=0;i<n+m+1;i++)
		Hess[i] = new double[i+1];

	get_starting_point(n, 1, xp, 0, zl, zu, m, 0, lamp);

	trace_on(tag_f);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];

	eval_obj(n,xa,obj_value);

	obj_value >>= dummy;

	trace_off();

	trace_on(tag_g);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];

	eval_constraints(n,xa,m,g);


	for(Index i=0;i<m;i++)
		g[i] >>= dummy;

	trace_off();

	trace_on(tag_L);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];
	for(Index i=0;i<m;i++)
		lam[i] <<= 1.0;
	sig <<= 1.0;

	eval_obj(n,xa,obj_value);

	obj_value *= sig;
	eval_constraints(n,xa,m,g);

	for(Index i=0;i<m;i++)
		obj_value += g[i]*lam[i];

	obj_value >>= dummy;

	trace_off();

	delete[] xa;
	delete[] xp;
	delete[] g;
	delete[] lam;
	delete[] lamp;
	delete[] zu;
	delete[] zl;

}
