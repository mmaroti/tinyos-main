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

void non_orthogonal() {

	const double eps1 = 3.0e-5;
	const double eps2 = 5.0e-5;

	const double m[] = {
		   -1.00, eps1, eps2,
			eps1,-1.00, eps1,
		   -eps1,-eps2, 1.00
	};

	double dummy[3];

	rotmat_to_angles(m, dummy);
}

int main() {

	non_orthogonal();

	const int N = 5000000;

	srand(7);

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

	cout << "Tests passed!" << endl;

	return 0;
}

