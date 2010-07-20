#include <iostream>
#include <fstream>
#include <cstdlib>
#include "InputData.hpp"
#include "DataImporter.hpp"

using namespace std;

typedef double NT;

void read_file(const char* filename, input& data) {

	data.dt    = NT(10.0/2048.0);
	data.g_ref = NT(-9.81);

	//--------------------------------------------------------------------------

	std::ifstream in(filename);

	if (!in.good()) {
		cerr << endl << "Failed to open input file!" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Reading input file: " << filename << endl;

	int& N = data.N;

	in >> N;

	if (N<1) {
		cerr << endl << "Invalid lenght!" << endl;
		exit(EXIT_FAILURE);
	}

	// FIXME Resources never released
	//----------------------------------------------------------------------

	double*& wx = data.wx;
	double*& wy = data.wy;
	double*& wz = data.wz;

	double*& acc_x = data.acc_x;
	double*& acc_y = data.acc_y;
	double*& acc_z = data.acc_z;

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
}
