#include <stdexcept>
#include "InputData.hpp"

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
	released = false;
}

input::input(const input& other) :

			acc_x_(other.acc_x_),
			acc_y_(other.acc_y_),
			acc_z_(other.acc_z_),

			wx_(other.wx_),
			wy_(other.wy_),
			wz_(other.wz_),

			N_(other.N_),

			dt_(other.dt_),
			g_ref_(other.g_ref_),
			released(other.released)
{
	if (released) {
		throw std::runtime_error("data already deleted");
	}
}

void input::release_resources() {

	if (released) {
		throw std::runtime_error("data already deleted");
	}
	released = true;

	delete[] acc_x_;
	delete[] acc_y_;
	delete[] acc_z_;

	delete[] wx_;
	delete[] wy_;
	delete[] wz_;
}
