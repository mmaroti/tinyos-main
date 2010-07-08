#include <hess_ari.hpp>     // Hessian differentiation arithmetic

using namespace cxsc;
using namespace std;

const int n_vars = 12;
						// R
HessType R_11(n_vars);
HessType R_12(n_vars);
HessType R_13(n_vars);

HessType R_21(n_vars);
HessType R_22(n_vars);
HessType R_23(n_vars);

HessType R_31(n_vars);
HessType R_32(n_vars);
HessType R_33(n_vars);
						// R0
HessType R0_11(n_vars);
HessType R0_12(n_vars);
HessType R0_13(n_vars);

HessType R0_21(n_vars);
HessType R0_22(n_vars);
HessType R0_23(n_vars);

HessType R0_31(n_vars);
HessType R0_32(n_vars);
HessType R0_33(n_vars);
						// Rn
HessType Rn_11(n_vars);
HessType Rn_12(n_vars);
HessType Rn_13(n_vars);

HessType Rn_21(n_vars);
HessType Rn_22(n_vars);
HessType Rn_23(n_vars);

HessType Rn_31(n_vars);
HessType Rn_32(n_vars);
HessType Rn_33(n_vars);
						// G
HessType G_11(n_vars);
HessType G_12(n_vars);
HessType G_13(n_vars);

HessType G_21(n_vars);
HessType G_22(n_vars);
HessType G_23(n_vars);

HessType G_31(n_vars);
HessType G_32(n_vars);
HessType G_33(n_vars);
						// A
interval A_11(n_vars);
interval A_12(n_vars);
interval A_13(n_vars);

interval A_21(n_vars);
interval A_22(n_vars);
interval A_23(n_vars);

interval A_31(n_vars);
interval A_32(n_vars);
interval A_33(n_vars);
						// C
HessType C_11(n_vars);
HessType C_12(n_vars);
HessType C_13(n_vars);

HessType C_21(n_vars);
HessType C_22(n_vars);
HessType C_23(n_vars);

HessType C_31(n_vars);
HessType C_32(n_vars);
HessType C_33(n_vars);
						// Cc
HessType Cc_11(n_vars);
HessType Cc_12(n_vars);
HessType Cc_13(n_vars);

HessType Cc_21(n_vars);
HessType Cc_22(n_vars);
HessType Cc_23(n_vars);

HessType Cc_31(n_vars);
HessType Cc_32(n_vars);
HessType Cc_33(n_vars);
						// b
interval b1(n_vars);
interval b2(n_vars);
interval b3(n_vars);
						// d
HessType d1(n_vars);
HessType d2(n_vars);
HessType d3(n_vars);
						// dc
HessType dc1(n_vars);
HessType dc2(n_vars);
HessType dc3(n_vars);
						// g
HessType gx(n_vars);
HessType gy(n_vars);
HessType gz(n_vars);
						// sum
HessType sx(n_vars);
HessType sy(n_vars);
HessType sz(n_vars);

//==============================================================================

HessType one(n_vars);
HessType dt(n_vars);
HessType g_ref(n_vars);

HessType* wx(0);
HessType* wy(0);
HessType* wz(0);

interval* ax(0);
interval* ay(0);
interval* az(0);

const int N(10);

void init() {

	one = interval(1);
	dt = interval(1.0/200.0);
	g_ref = interval(9.81);

	//--------------------------------------------

	wx = new HessType[N];

	for (int i=0; i<N; ++i)
		Resize(wx[i], n_vars);

	wy = new HessType[N];

	for (int i=0; i<N; ++i)
		Resize(wy[i], n_vars);

	wz = new HessType[N];

	for (int i=0; i<N; ++i)
		Resize(wz[i], n_vars);

	//--------------------------------------------------

	ax = new interval[n_vars];
	ay = new interval[n_vars];
	az = new interval[n_vars];

	//---------------------------------------------------

	A_11 = interval(1.0);
	A_12 = interval(0.1);
	A_13 = interval(0.2);

	A_21 = interval(0.3);
	A_22 = interval(1.0);
	A_23 = interval(0.1);

	A_31 = interval(0.2);
	A_32 = interval(0.1);
	A_33 = interval(1.0);

	//----------------------------------------------

	Cc_11 = interval(1.0);
	Cc_12 = interval(0.1);
	Cc_13 = interval(0.2);

	Cc_21 = interval(0.3);
	Cc_22 = interval(1.0);
	Cc_23 = interval(0.1);

	Cc_31 = interval(0.2);
	Cc_32 = interval(0.1);
	Cc_33 = interval(1.0);

	//-----------------------------------------------

	b1 = interval(0.1);
	b2 = interval(0.2);
	b3 = interval(0.3);

	//-----------------------------------------------

	dc1 = interval(0.3);
	dc2 = interval(0.1);
	dc3 = interval(0.2);

	//---------------------------------------------------

	R0_11 = interval(1.0);
	R0_12 = interval(0.0);
	R0_13 = interval(0.0);

	R0_21 = interval(0.0);
	R0_22 = interval(1.0);
	R0_23 = interval(0.0);

	R0_31 = interval(0.0);
	R0_32 = interval(0.0);
	R0_33 = interval(1.0);

	//---------------------------------------------------

	for (int i=0; i<N; ++i) {

		wx[i] = interval( 1.0+0.1*i);
		wy[i] = interval(-0.5+0.2*i);
		wz[i] = interval( 0.7-0.3*i);

		ax[i] = interval(0.2+0.1*i);
		ay[i] = interval(0.8-0.1*i);
		az[i] = 9.81 + interval(0.1*i);
	}

}

void set_sum_and_R0() {

	sx = interval(0);
	sy = interval(0);
	sz = interval(0);

	R_11 = R0_11; R_12 = R0_12; R_13 = R0_13;
	R_21 = R0_21; R_22 = R0_22; R_23 = R0_23;
	R_31 = R0_31; R_32 = R0_32; R_33 = R0_33;
}

void compute_G(int i, const HTvector& x) {

	C_11 = Cc_11 + x[1];
	C_12 = Cc_12 + x[2];
	C_13 = Cc_13 + x[3];

	C_21 = Cc_21 + x[4];
	C_22 = Cc_22 + x[5];
	C_23 = Cc_23 + x[6];

	C_31 = Cc_31 + x[7];
	C_32 = Cc_32 + x[8];
	C_33 = Cc_33 + x[9];

	d1 = dc1 + x[10];
	d2 = dc2 + x[11];
	d3 = dc3 + x[12];

	HessType w_x = (C_11*wx[i]+C_12*wy[i]+C_13*wz[i]+d1)*dt;
	HessType w_y = (C_21*wx[i]+C_22*wy[i]+C_23*wz[i]+d2)*dt;
	HessType w_z = (C_31*wx[i]+C_32*wy[i]+C_33*wz[i]+d3)*dt;

	G_11 =  one;
	G_12 = -w_z;
	G_13 =  w_y;

	G_21 =  w_z;
	G_22 =  one;
	G_23 = -w_x;

	G_31 = -w_y;
	G_32 =  w_x;
	G_33 =  one;

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

}

void compute_g(int i) {

	gx = A_11*ax[i]+A_12*ay[i]+A_13*az[i]+b1;
	gy = A_21*ax[i]+A_22*ay[i]+A_23*az[i]+b2;
	gz = A_31*ax[i]+A_32*ay[i]+A_33*az[i]+b3;
}

void sum_Ri_gi() {
	// TODO Scaling factor?
	sx = sx + (R_11*gx+R_12*gy+R_13*gz);
	sy = sy + (R_21*gx+R_22*gy+R_23*gz);
	sz = sz + (R_31*gx+R_32*gy+R_33*gz);
	return;
}

HessType f(const HTvector& x)
{

	set_sum_and_R0();

	for (int i=1; i<N; ++i) {

		compute_G(i-1, x);

		update_R(); // R(i)=R(i-1)*G(i-1)

		// TODO Normalize

		compute_g(i);

		sum_Ri_gi();

	}

	return sqr(sx)+sqr(sy)+sqr(sz/N-g_ref);
}

int main ()
{

	init();

	interval  fx;
	ivector   x(n_vars), Gfx(n_vars);
	imatrix   Hfx(n_vars, n_vars);

	for (int i=1; i<=n_vars; ++i)
		x[i] = interval(-0.9 + i*0.11);


	fghEvalH(f,x,fx,Gfx,Hfx);

    cout << "x: " << endl << x << endl;
    cout << "f(x):	" << endl << fx << endl;
    cout << "Gf(x): " << endl << Gfx << endl;

	return 0;
}
