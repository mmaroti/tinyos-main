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
#include <fstream>
#include <cstdlib>
#include "InputData.hpp"
#include "DataImporter.hpp"

using namespace std;

typedef double NT;

namespace gyro {

input* read_stdin()	 {

	double dt    = NT(10.0/2048.0);
	double g_ref = NT(-9.81);

	int N = -1;

	cin >> N;

	if (N<1) {
		cerr << endl << "Invalid lenght!" << endl;
		exit(EXIT_FAILURE);
	}

	double* time_stamp = new NT[N];

	double* wx = new NT[N];
	double* wy = new NT[N];
	double* wz = new NT[N];

	double* acc_x = new NT[N];
	double* acc_y = new NT[N];
	double* acc_z = new NT[N];

	for (int i=0; i<N; ++i) {

		cin >> time_stamp[i];

		cin >> acc_x[i];
		cin >> acc_y[i];
		cin >> acc_z[i];

		cin >> wx[i];
		cin >> wy[i];
		cin >> wz[i];

	}

	return new input(time_stamp, acc_x, acc_y, acc_z, wx, wy, wz, N, dt, g_ref);

}

input* read_file(const char* filename) {

	double dt    = NT(10.0/2048.0);
	double g_ref = NT(-9.81);

	std::ifstream in(filename);

	if (!in.good()) {
		cerr << endl << "Failed to open input file!" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Reading input file: " << filename << endl;

	int N = -1;

	in >> N;

	if (N<1) {
		cerr << endl << "Invalid lenght!" << endl;
		exit(EXIT_FAILURE);
	}

	double* time_stamp = new NT[N];

	double* wx = new NT[N];
	double* wy = new NT[N];
	double* wz = new NT[N];

	double* acc_x = new NT[N];
	double* acc_y = new NT[N];
	double* acc_z = new NT[N];

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

	return new input(time_stamp, acc_x, acc_y, acc_z, wx, wy, wz, N, dt, g_ref);
}

}


