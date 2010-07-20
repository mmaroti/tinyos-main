#ifndef INPUTDATA_HPP_
#define INPUTDATA_HPP_

struct input {

	// FIXME Try to make them const

	double* acc_x;
	double* acc_y;
	double* acc_z;

	double* wx;
	double* wy;
	double* wz;

	int N;

	double dt;
	double g_ref;
};

#endif
