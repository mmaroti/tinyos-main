#include <fstream>
#include <iomanip>
#include "ADOL-C_NLP.hpp"

using namespace Ipopt;

namespace {

  const int N_VARS = 12;
  const int N_CONS = 0;
}

typedef double NT;

template<typename T>
class glob {

private:

  T R_11; T R_12; T R_13;
  T R_21; T R_22; T R_23;
  T R_31; T R_32; T R_33;

  T R0_11; T R0_12; T R0_13;
  T R0_21; T R0_22; T R0_23;
  T R0_31; T R0_32; T R0_33;

  T Rn_11; T Rn_12; T Rn_13;
  T Rn_21; T Rn_22; T Rn_23;
  T Rn_31; T Rn_32; T Rn_33;

  T G_11; T G_12; T G_13;
  T G_21; T G_22; T G_23;
  T G_31; T G_32; T G_33;

  NT A_11; NT A_12; NT A_13;
  NT A_21; NT A_22; NT A_23;
  NT A_31; NT A_32; NT A_33;

  T C_11; T C_12; T C_13;
  T C_21; T C_22; T C_23;
  T C_31; T C_32; T C_33;

  T Cc_11; T Cc_12; T Cc_13;
  T Cc_21; T Cc_22; T Cc_23;
  T Cc_31; T Cc_32; T Cc_33;

  NT b1; NT b2; NT b3;
  T  d1; T  d2; T  d3;
  T dc1; T dc2; T dc3;
  T gx; T gy; T gz;
  T sx; T sy; T sz;
  // FIXME
  T gx0, gy0, gz0;

  //============================================================================

  T half; T one; T three; T dt; T g_ref;

  NT* wx; NT* wy; NT* wz;

  NT* ax; NT* ay; NT* az;

  int N;

  const bool VERBOSE;

  //============================================================================

  void set_sum_and_R0() {

	  sx = NT(0);
	  sy = NT(0);
	  sz = NT(0);

	  R_11 = R0_11; R_12 = R0_12; R_13 = R0_13;
	  R_21 = R0_21; R_22 = R0_22; R_23 = R0_23;
	  R_31 = R0_31; R_32 = R0_32; R_33 = R0_33;
  }

  void compute_G(int i, const T* const x) {

	  C_11 = Cc_11 + x[0];
	  C_12 = Cc_12 + x[1];
	  C_13 = Cc_13 + x[2];

	  C_21 = Cc_21 + x[3];
	  C_22 = Cc_22 + x[4];
	  C_23 = Cc_23 + x[5];

	  C_31 = Cc_31 + x[6];
	  C_32 = Cc_32 + x[7];
	  C_33 = Cc_33 + x[8];

	  d1 = dc1 + x[ 9];
	  d2 = dc2 + x[10];
	  d3 = dc3 + x[11];

	  if (VERBOSE) {
	    cout << endl;
	    cout << "gyro x, y, z" << endl;
	    cout << wx[i] << '\t' << wy[i] << '\t' << wz[i] << endl;
	  }

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

  void compute_g(int i) {

	  gx = A_11*ax[i]+A_12*ay[i]+A_13*az[i]+b1;
	  gy = A_21*ax[i]+A_22*ay[i]+A_23*az[i]+b2;
	  gz = A_31*ax[i]+A_32*ay[i]+A_33*az[i]+b3;

	  if (VERBOSE) {
	    cout << endl;
	    cout << "a(i)" << endl;
	    cout << gx << '\t' << gy << '\t' << gz << endl;
	  }
  }

  void sum_Ri_gi() {

	  T acc_x = R_11*gx+R_12*gy+R_13*gz;
	  T acc_y = R_21*gx+R_22*gy+R_23*gz;
	  T acc_z = R_31*gx+R_32*gy+R_33*gz;

	  if (VERBOSE) {
	    cout << endl;
	    cout << "R(i)*a(i)" << endl;
	    cout << acc_x << ' ' << acc_y << ' ' << acc_z << endl;
	  }

	  // TODO Scaling factor?
	  sx = sx + acc_x;
	  sy = sy + acc_y;
	  sz = sz + acc_z;
	  return;
  }

  T objective() {
    return (sx/N-gx0)*(sx/N-gx0) + (sy/N-gy0)*(sy/N-gy0) + (sz/N-gz0)*(sz/N-gz0);
  }

public:

  explicit glob(const char* const filename, bool verbose = false) : VERBOSE(verbose) {
	half  = NT(0.5);
	one   = NT(1);
	three = NT(3);
	dt    = NT(10.0/2048.0);
	g_ref = NT(9.81);

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

	//--------------------------------------------------------------------------

	wx = new NT[N];
	wy = new NT[N];
	wz = new NT[N];

	//--------------------------------------------------

	ax = new NT[N];
	ay = new NT[N];
	az = new NT[N];

	//---------------------------------------------------

	A_11 = NT(1.0);
	A_12 = NT(0.0);
	A_13 = NT(0.0);

	A_21 = NT(0.0);
	A_22 = NT(1.0);
	A_23 = NT(0.0);

	A_31 = NT(0.0);
	A_32 = NT(0.0);
	A_33 = NT(1.0);

	//----------------------------------------------

	Cc_11 = NT(1.0);
	Cc_12 = NT(0.0);
	Cc_13 = NT(0.0);

	Cc_21 = NT(0.0);
	Cc_22 = NT(-1.0); // <--
	Cc_23 = NT(0.0);

	Cc_31 = NT(0.0);
	Cc_32 = NT(0.0);
	Cc_33 = NT(1.0);

	//-----------------------------------------------

	b1 = NT(0.0);
	b2 = NT(0.0);
	b3 = NT(0.0);

	//-----------------------------------------------

	dc1 = NT(0.0);
	dc2 = NT(0.0);
	dc3 = NT(0.0);

	//---------------------------------------------------

	R0_11 = NT(1.0);
	R0_12 = NT(0.0);
	R0_13 = NT(0.0);

	R0_21 = NT(0.0);
	R0_22 = NT(1.0);
	R0_23 = NT(0.0);

	R0_31 = NT(0.0);
	R0_32 = NT(0.0);
	R0_33 = NT(1.0);

	//---------------------------------------------------

	double dummy(0.0);

	for (int i=0; i<N; ++i) {

		in >> ax[i];
		in >> ay[i];
		in >> az[i];

		in >> dummy; in >> dummy; in >> dummy; in >> dummy;


		in >> wx[i];
		in >> wy[i];
		in >> wz[i];

		if (!in.good()) {
		  cerr << endl << "Problems on reading from input" << endl;
		  exit(EXIT_FAILURE);
		}
	}

	compute_g(0);

	gx0 = gx; gy0 = gy; gz0 = gz;
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

		  compute_g(i);

		  sum_Ri_gi();

	  }

	  return objective();
  }

};

void dbg_objective(const char* const filename) {


  glob<double> obj(filename, true);
  /*
  double x[N_VARS];

  for (int i=0; i<N_VARS; ++i)
	  x[i] = 0.0;

  x[0] = -0.356778;
  x[1] = 1.07415;
  x[2] = -0.782412;
  x[3] = -0.604513;
  x[4] = 1.00259;
  x[5] = -2.40572;
  x[6] = -0.0529504;
  x[7] = 0.134043;
  x[8] = -0.236761;
  x[9] = -2.40383;
  x[10] = -7.99147;
  x[11] = -0.714655;*/

  double x[] = {
  0.00863561, -0.0278826,  0.0198411,
  0.00925871, -2.03525,   -0.0023857,
  -0.0145452,  0.0442004, -0.0433523,
  -0.0689957, -0.270561, 0.20561 };

  assert (sizeof(x)/sizeof(double) == N_VARS);

  double z = obj.f(x);

  cout << "===========================================================" << endl;
  cout << "Objective: " << z << endl;
  return;
}

template<class T> bool  MyADOLC_NLP::eval_obj(Index n, const T *x, T& obj_value)
{
  static glob<T> gv("manual");

  obj_value = gv.f(x);

  return true;
}

template<class T> bool  MyADOLC_NLP::eval_constraints(Index n, const T *x, Index m, T* g)
{
  return true;
}

bool MyADOLC_NLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
                            Index m, Number* g_l, Number* g_u)
{

	for (Index i=0; i<n; i++) {
		x_l[i] = -1.0;
		x_u[i] =  1.0;
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
