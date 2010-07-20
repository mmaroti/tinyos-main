#include <iostream>
#include "IpIpoptApplication.hpp"
#include "GyroNLP.hpp"
#include "Optimizer.hpp"

using namespace std;
using namespace Ipopt;

bool Optimizer::run() {

	SmartPtr<IpoptApplication> app = new IpoptApplication();

	ApplicationReturnStatus status(app->Initialize());

	if (status != Solve_Succeeded) {
		cerr << "Error during initialization of IPOPT!" << endl;
		return false;
	}

	SmartPtr<TNLP> nlp = new GyroNLP(data, os);

	status = app->OptimizeTNLP(nlp);

	if (status != Solve_Succeeded) {
		cerr << "Error during optimization!" << endl;
		return false;
	}

	// FIXME

	return true;
}
