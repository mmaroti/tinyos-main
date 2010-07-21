#include <iostream>
#include <fstream>
#include <cstdlib>
#include "InputData.hpp"
#include "DataImporter.hpp"

using namespace std;

typedef double NT;

namespace gyro {

input read_file(const char* filename) {

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

	return input(acc_x, acc_y, acc_z, wx, wy, wz, N, dt, g_ref);
}

}


