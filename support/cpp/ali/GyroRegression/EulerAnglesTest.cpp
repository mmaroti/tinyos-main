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
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include "EulerAngles.hpp"

using namespace std;
using namespace gyro;

namespace dbg {

	void consistent(const double angles_deg[3]);
	void consistent_in_range(const double angles_deg[3]);
	void equal_arrays(const double a[], const double b[], int len, const char* name);
}

int random_deg() {
	const int a = (rand() % 721) - 360;
	assert ( (-360 <= a) && (a <= 360) );
	return a;
}

void test(double Y = random_deg()) {

	double X = random_deg();
	double Z = random_deg();
	double angles[] = { X, Y, Z };
	dbg::consistent(angles);
}

int random_deg_xz() {
	const int a = (rand() % 360) - 179;
	assert ( (-179 <= a) && (a <= 180) );
	return a;
}

int random_deg_y() {
	const int a = (rand() % 179) - 89;
	assert ( (-89 <= a) && (a <= 89) );
	return a;
}

void test_in_range() {
	double X = random_deg_xz();
	double Y = random_deg_y();
	double Z = random_deg_xz();
	double angles[] = { X, Y, Z };
	dbg::consistent_in_range(angles);
}

double random_coordinate() {

	const double x = ((rand() % 2001) - 1000) / 100.1;
	assert (-10 < x && x < 10);
	return x;
}

void test_rotation() {

	const double angles[] = { random_deg(), random_deg(), random_deg() };

	double r[9];

	angles_deg_to_rotmat(angles, r);

	const double u[] =
		{ random_coordinate(), random_coordinate(), random_coordinate() };

	double v[3];

	rotate_vector(r, u, v);

	double w[3];

	inverse_rot_vector(r, v, w);

	dbg::equal_arrays(u, w, 3, "Random rotated vector");
}

void non_orthogonal() {

	const double eps1 = 3.0e-5;
	const double eps2 = 5.0e-5;

	const double m[] = {
		   -1.00, eps1, eps2,
			eps1,-1.00, eps1,
		   -eps1,-eps2, 1.00
	};

	double dummy[3];

	rotmat_to_angles_deg(m, dummy);
}

void M_test(const double a[3]) {

	double M[9];

	get_M(a, M);

	double r[3];

	rotate_vector(M, a, r);

	cout << "a rotated: " << r[0] << '\t' << r[1] << '\t' << r[2] << endl;

	// FIXME Check length
}

void M_test() {

	cout << "Testing M" << endl;

	const double a[] = {-3,-5, 10 };

	M_test(a);

	const double b[] = { -6.0e-7, 2, 5.0e-7 };

	M_test(b);

	const double c[] = { -1, 3, -2 };

	M_test(c);

	const double d[] = { 7, 5, 6 };

	M_test(d);

	const double e[] = { 8,-3, 5 };

	M_test(e);

	return;
}

void Euler_check_if_bug() {

	double m[] = { -0.00281969, -0.942722,   0.333568,
			       0.000693726,  0.333568,   0.942726,
			       -0.999996,    0.0028896, -0.000286568 };

	const double a[] = { 1, 1, 1};

	double b1[3];

	rotate_vector(m, a, b1);

	double angle[3];

	rotmat_to_angles_deg(m, angle);

	double m2[9];

	angles_deg_to_rotmat(angle, m2);

	double b2[3];

	rotate_vector(m2, a, b2);

	dbg::equal_arrays(b1, b2, 3, "degenerate case");

	cout << "Euler degeneracy test passed" << endl;
}

int main() {

	// Set TOL_EQUAL to 1.0e-4 if enabled
	//Euler_check_if_bug();

	M_test();

	non_orthogonal();

	const int N = 1000000;

	srand(127);

	cout << "Testing degeneracy, Y = 90 deg" << endl;

	for (int i=0; i<N; ++i) {
		test(90);
	}

	cout << "Testing degeneracy, Y = -90 deg" << endl;

	for (int i=0; i<N; ++i) {
		test(-90);
	}

	cout << "Testing random angles" << endl;

	for (int i=0; i<N; ++i) {
		test();
	}

	cout << "Testing in range random angles" << endl;

	for (int i=0; i<N; ++i) {
		test_in_range();
	}

	cout << "Testing random rotations" << endl;

	for (int i=0; i<N; ++i) {
		test_rotation();
	}

	cout << "Tests passed!" << endl;

	return 0;
}

