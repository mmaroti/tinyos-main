#include <stdexcept>
#include "InputData.hpp"

namespace gyro {

input::input(const double* acc_x,
			const double* acc_y,
			const double* acc_z,
			const double* wx,
			const double* wy,
			const double* wz,
			int N,
			double dt,
			double g_ref) :

			acc_x_(acc_x),
			acc_y_(acc_y),
			acc_z_(acc_z),

			wx_(wx),
			wy_(wy),
			wz_(wz),

			N_(N),

			dt_(dt),
			g_ref_(g_ref)
{

}

input::~input() {

	delete[] acc_x_;
	delete[] acc_y_;
	delete[] acc_z_;

	delete[] wx_;
	delete[] wy_;
	delete[] wz_;
}

}
