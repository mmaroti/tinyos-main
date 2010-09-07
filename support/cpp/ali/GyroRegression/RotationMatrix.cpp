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

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>
#include "ObjectiveEvaluator.hpp"
#include "RotationMatrix.hpp"

using namespace std;

namespace gyro {

RotationMatrix::RotationMatrix(	const input& data,
		const double* const x,
		ostream& log,
		bool verbose) :
		R(new double[9*data.N()]),
		g_err(new double[3*data.N()]),
		//Rt_g( new double[3*data.N()]),
		//a(    new double[3*data.N()]),
		//alpha(new double[3*data.N()]),
		//beta( new double[3*data.N()]),
		N(data.N())
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

double RotationMatrix::at(const int measurement, const int i, const int j) const {

	assert(0<=measurement && measurement<N);
	assert(1<=i && i<=3);
	assert(1<=j && j<=3);

	const int index = 9*measurement + 3*(i-1) + (j-1);
	return R[index];
}

void RotationMatrix::dump_matrices(ostream& log) const {

	log << fixed;
	log << setprecision(4);

	for (int i=0; i<N; ++i) {
		log << endl;
		log << "Sample #" << i << endl;
		log << this->at(i,1,1) << '\t' << this->at(i,1,2) << '\t' << this->at(i,1,3) << endl;
		log << this->at(i,2,1) << '\t' << this->at(i,2,2) << '\t' << this->at(i,2,3) << endl;
		log << this->at(i,3,1) << '\t' << this->at(i,3,2) << '\t' << this->at(i,3,3) << endl;
	}
}

void RotationMatrix::dump_g_err(ostream& log) const {

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

void RotationMatrix::dump_angles(const input& data,
								std::ostream& log ) const
{
	const double g_ref = data.g_ref();

	double  a[4];
	double  b[4];
	double nb[4];
	double beta[4];

	for (int i=0; i<N; ++i) {

		for (int j=1; j<=3; ++j) {

			nb[j] = at(i,3,j);

			if (nb[j] > 1.0)
				nb[j] = 1.0;
			else if (nb[j] < -1.0)
				nb[j] = -1.0;

			beta[j] = asin(nb[j])*(180.0/M_PI);

			b[j] = at(i,3,j)*g_ref;
		}

		a[1] = data.acc_x()[i];
		a[2] = data.acc_y()[i];
		a[3] = data.acc_z()[i];

		double at = sqrt(pow(a[1], 2)+pow(a[2], 2)+pow(a[3], 2));

		double ax = a[1]/at;
		double ay = a[2]/at;
		double az = a[3]/at;

		if      (ax >  1.0) ax =  1.0;
		else if (ax < -1.0) ax = -1.0;

		if      (ay >  1.0) ay =  1.0;
		else if (ay < -1.0) ay = -1.0;

		if      (az >  1.0) az =  1.0;
		else if (az < -1.0) az = -1.0;

		double alpha_x = asin(ax)*(180.0/M_PI);
		double alpha_y = asin(ay)*(180.0/M_PI);
		double alpha_z = asin(az)*(180.0/M_PI);

		log << a[1] << '\t' << a[2] << '\t' << a[3] << '\t';
		log << b[1] << '\t' << b[2] << '\t' << b[3] << '\t';
		log << alpha_x << '\t' << alpha_y << '\t' << alpha_z << '\t';
		log << beta[1] << '\t' << beta[2] << '\t' << beta[3] << '\t';
		log << endl;


	}

	log << endl;
}

}

