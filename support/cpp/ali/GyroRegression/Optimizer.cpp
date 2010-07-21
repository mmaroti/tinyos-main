#include <iostream>
#include <cmath>
#include <stdexcept>
#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"
#include "GyroNLP.hpp"
#include "Optimizer.hpp"
#include "InputData.hpp"

using namespace std;
using namespace Ipopt;

namespace gyro {

Optimizer::Optimizer(const input& data, std::ostream& os, bool verbose) {

	SmartPtr<IpoptApplication> app = new IpoptApplication();

	app->Options()->SetNumericValue("tol", 1.0e-3);
	app->Options()->SetStringValue("hessian_approximation", "limited-memory");
	app->Options()->SetStringValue("limited_memory_update_type", "bfgs");

	ApplicationReturnStatus status(app->Initialize("ipopt.opt"));

	if (status != Solve_Succeeded) {
		throw runtime_error("Error during initialization of IPOPT!");
	}

	SmartPtr<TNLP> nlp = new GyroNLP(data, os, verbose);

	status = app->OptimizeTNLP(nlp);

	if (status != Solve_Succeeded && status != Solved_To_Acceptable_Level) {
		throw runtime_error("Error during optimization!");
	}

	g_computed = std::sqrt(-app->Statistics()->FinalObjective());

	g_error = g_computed - (fabs(data.g_ref()));

	const GyroNLP* const gyro_nlp =
			static_cast<const GyroNLP* const> (GetRawPtr(nlp));

	const double* x = gyro_nlp->solution();

	for (int i=0; i<NUMBER_OF_VARIABLES; ++i)
		minimizer[i] = x[i];
}

}
