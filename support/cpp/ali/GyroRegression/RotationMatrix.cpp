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

// FIXME Remove fstream; add new exit-code for runtime-error
//#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <assert.h>
#include "ObjectiveEvaluator.hpp"
#include "RotationMatrix.hpp"
#include "EulerAngles.hpp"

using namespace std;

namespace dbg {
	void orthogonality(const double m[9]);
}

namespace {

enum {
	X, Y, Z
};

enum {
	R11, R12, R13,
	R21, R22, R23,
	R31, R32, R33
};

void set_a_alpha(const gyro::Input& data, int i, double a[3], double alpha[3]) {

	using gyro::RAD;

	a[X] = data.acc_x()[i];
	a[Y] = data.acc_y()[i];
	a[Z] = data.acc_z()[i];

	double at = sqrt(pow(a[X], 2)+pow(a[Y], 2)+pow(a[Z], 2));

	double ax = a[X]/at;
	double ay = a[Y]/at;
	double az = a[Z]/at;

	if      (ax >  1.0) ax =  1.0;
	else if (ax < -1.0) ax = -1.0;

	if      (ay >  1.0) ay =  1.0;
	else if (ay < -1.0) ay = -1.0;

	if      (az >  1.0) az =  1.0;
	else if (az < -1.0) az = -1.0;

	alpha[X] = asin(ax)*RAD;
	alpha[Y] = asin(ay)*RAD;
	alpha[Z] = asin(az)*RAD;
}

}

namespace gyro {

RotationMatrix::RotationMatrix(	const Input& data,
		const double* const x,
		ostream& log,
		bool verbose) :
		R(new double[9*data.N()]),
		g_err(new double[3*data.N()]),
		N(data.N())
{

	ObjEval<double> obj(data, log, verbose);

	obj.f(x);

	compute_M(obj.s_x(), obj.s_y(), obj.s_z(), data);

	obj.set_M(R, g_err);

	obj.f(x);

	// TODO Acceptance test: (s_x,s_y,s_z) should be (0,0,g)

	objective_accept(obj.s_x(), obj.s_y(), obj.s_z());

	orthogonality_accept();

	/*
	ofstream out("sep01mat");
	dump_angles(data, out);

	out.close();
	out.open("g_err");
	dump_g_err(out);

	out.close();
	out.open("objective");
	out << "sx: " << obj.s_x() << endl;
	out << "sy: " << obj.s_y() << endl;
	out << "sz: " << obj.s_z() << endl;
	*/
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

	log << scientific;
	log << setprecision(16);

	for (int i=0; i<N; ++i) {
		log << at(i,1,1) << '\n' << at(i,1,2) << '\n' << at(i,1,3) << '\n';
		log << at(i,2,1) << '\n' << at(i,2,2) << '\n' << at(i,2,3) << '\n';
		log << at(i,3,1) << '\n' << at(i,3,2) << '\n' << at(i,3,3) << '\n';
	}

	log << flush;
}

void RotationMatrix::dump_g_err(ostream& log) const {

	for (int i=0; i<N; ++i) {
		const int k = 3*i;
		log << g_err[k] << '\t' << g_err[k+1] << '\t' << g_err[k+2] << endl;
	}
}

void RotationMatrix::compute_M(const double ax,
		const double ay,
		const double az,
		const Input& data)
{
	const double a[] = { ax, ay, az };

	get_M(a, R);

	dbg::orthogonality(R);

	const double a0[] = { data.acc_x()[0], data.acc_y()[0], data.acc_z()[0] };

	rotate_vector(R, a0, g_err);

	g_err[Z] -= data.g_ref();
}

const double* RotationMatrix::matrix_at(int i) const {

	assert(0<=i && i<N);

	return R+(9*i);
}

void RotationMatrix::orthogonality_accept() const {

	try {
		for (int i=0; i<N; ++i) {
			dbg::orthogonality(matrix_at(i));
		}
	}
	catch(runtime_error& ) {
		cerr << "Error: some of the computed rotation matrices ";
		cerr << "are not orthogonal!" << endl;
		exit(ERROR_ORTHOGONALITY);
	}
}

void RotationMatrix::objective_accept(double sx, double sy, double sz) const {

	const double TOL_XY = 1.0e-6;
	const double TOL_Z  = fabs(0.15*GRAVITY);

	if (fabs(sx) > TOL_XY || fabs(sy) > TOL_XY || fabs(GRAVITY-sz) > TOL_Z) {
		cerr << "Error: the computed gravitational acceleration significantly ";
		cerr << "differs from the reference value!" << endl;
		cerr << "( " << sx << ", " << sy << ", " << sz << ")" << endl;
		exit(ERROR_OBJECTIVE);
	}
}

void RotationMatrix::dump_angles(const Input& data,
								std::ostream& log ) const
{
	const double g[] = { 0, 0, data.g_ref() };

	double  a[3];
	double  b[3];
	double alpha[3];
	double  beta[3];
	double euler[3];

	for (int i=0; i<N; ++i) {

		const double* const m = matrix_at(i);

		try {
			inverse_rot_vector(m, g, b);

			rotmat_to_asin_angles_deg(m, beta);
			// TODO Difference between xyz and yzx conventions? -> Finish visualization first!
			rotmat_to_angles_deg(m, euler);
		}
		catch (runtime_error& e) {
			log << "Runtime error at sample " << i << " of " << N << endl;
			log << "Message: " << e.what() << endl;
			throw;
		}

		set_a_alpha(data, i, a, alpha);

		log << a[X] << '\t' << a[Y] << '\t' << a[Z] << '\t';
		log << b[X] << '\t' << b[Y] << '\t' << b[Z] << '\t';
		log << alpha[X] << '\t' << alpha[Y] << '\t' << alpha[Z] << '\t';
		log <<  beta[X] << '\t' <<  beta[Y] << '\t' <<  beta[Z] << '\t';
		log << euler[X] << '\t' << euler[Y] << '\t' << euler[Z] << '\t';
		log << endl;
	}

	log << endl;
}

}

