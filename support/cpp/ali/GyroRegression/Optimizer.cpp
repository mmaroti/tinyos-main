#include <iostream>
#include <stdexcept>
#include "IpIpoptApplication.hpp"
#include "GyroNLP.hpp"
#include "Optimizer.hpp"

using namespace std;
using namespace Ipopt;

Optimizer::Optimizer(const input& data, std::ostream& os, bool verbose) {

	SmartPtr<IpoptApplication> app = new IpoptApplication();

	ApplicationReturnStatus status(app->Initialize());

	if (status != Solve_Succeeded) {
		throw runtime_error("Error during initialization of IPOPT!");
	}

	SmartPtr<TNLP> nlp = new GyroNLP(data, os, verbose);

	// FIXME Make bfgs default!
	status = app->OptimizeTNLP(nlp);

	if (status != Solve_Succeeded && status != Solved_To_Acceptable_Level) {
		throw runtime_error("Error during optimization!");
	}

	// FIXME Implement M calculation

}
