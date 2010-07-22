#ifndef ROTATIONMATRIX_HPP_
#define ROTATIONMATRIX_HPP_

#include <iosfwd>

namespace gyro {

class input;

class RotationMatrix {

public:

	RotationMatrix(	const input& data,
					const double* const x,
					std::ostream& log = std::cout,
					bool verbose = false);

	double at(const int measurement, const int i, const int j) const;

	void dump_matrices(std::ostream& log = std::cout) const;

	void dump_g_err(std::ostream& log = std::cout) const;

	~RotationMatrix();

private:

	RotationMatrix(const RotationMatrix& );

	RotationMatrix& operator=(const RotationMatrix& );

	void compute_M(	const double ax,
					const double ay,
					const double az,
					const input& data);

	double* const R;
	double* const g_err;
	const int N;

};

}

#endif

