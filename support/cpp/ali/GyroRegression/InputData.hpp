#ifndef INPUTDATA_HPP_
#define INPUTDATA_HPP_

class input {

public:

	input(	const double* acc_x,
			const double* acc_y,
			const double* acc_z,
			const double* wx,
			const double* wy,
			const double* wz,
			int N,
			double dt,
			double g_ref);

	input(const input& other);

	const double* acc_x() const { return acc_x_; }
	const double* acc_y() const { return acc_y_; }
	const double* acc_z() const { return acc_z_; }

	const double* wx() const { return wx_; }
	const double* wy() const { return wy_; }
	const double* wz() const { return wz_; }

	int N() const { return N_; }

	double dt() const    { return dt_; }
	double g_ref() const { return g_ref_; }

	void release_resources();

private:

	input& operator=(const input& );

	const double* const acc_x_;
	const double* const acc_y_;
	const double* const acc_z_;

	const double* const wx_;
	const double* const wy_;
	const double* const wz_;

	const int N_;

	const double dt_;
	const double g_ref_;

	bool released;
};

#endif
