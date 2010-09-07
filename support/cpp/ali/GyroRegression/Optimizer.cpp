/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Ali Baharev
*/

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
	app->Options()->SetIntegerValue("print_level", 0);
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
