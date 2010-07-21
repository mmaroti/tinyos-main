#ifndef OPTIMIZER_HPP_
#define OPTIMIZER_HPP_

#include <iosfwd>
#include "CompileTimeConstants.hpp"

namespace gyro {

class input;

class Optimizer {

public:

	Optimizer(const input& data, std::ostream& os, bool verbose = false);

	const double* solution() const { return minimizer; }

	int n_vars() const { return NUMBER_OF_VARIABLES; }

	double computed_g() const { return g_computed; }

	double error_in_g() const { return g_error; }

private:

	double minimizer[NUMBER_OF_VARIABLES];

	double g_computed;

	double g_error;

};

}

#endif

