#include <iostream>
#include <cmath>
#include "ObjectiveEvaluator.hpp"
#include "RotationMatrix.hpp"

using namespace std;

namespace gyro {

RotationMatrix::RotationMatrix(	const input& data,
		const double* const x,
		ostream& log,
		bool verbose) :
		R(new double[9*data.N()]), g_err(new double[3*data.N()]), N(data.N())
{

	ObjEval<double> obj(data, log, verbose);

	obj.f(x);

	compute_M(obj.s_x(), obj.s_y(), obj.s_z(), data);

	obj.set_M(R, g_err);

	obj.f(x);

}

RotationMatrix::~RotationMatrix() {

	delete[] R;
	delete[] g_err;
}

void RotationMatrix::dump_g_err(ostream& log) {

	for (int i=0; i<N; ++i) {
		const int k = 3*i;
		log << g_err[k] << '\t' << g_err[k+1] << '\t' << g_err[k+2] << endl;
	}
}

void RotationMatrix::compute_M(	const double ax,
		const double ay,
		const double az,
		const input& data)
{

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

			const double M11 = ux2+uy2*c;
			const double M12 = uxy*(1.0-c);
			const double M13 = uy*s;

			const double M21 = M12;
			const double M22 = uy2+ux2*c;
			const double M23 =-ux*s;

			const double M31 =-M13;
			const double M32 =-M23;
			const double M33 = c;

			R[0] = M11; R[1] = M12; R[2] = M13;
			R[3] = M21; R[4] = M22; R[5] = M23;
			R[6] = M31; R[7] = M32; R[8] = M33;

			const double ax0 = data.acc_x()[0];
			const double ay0 = data.acc_y()[0];
			const double az0 = data.acc_z()[0];

			g_err[0] = M11*ax0+M12*ay0+M13*az0;
			g_err[1] = M21*ax0+M22*ay0+M23*az0;
			g_err[2] = M31*ax0+M32*ay0+M33*az0 - data.g_ref();

}

}

