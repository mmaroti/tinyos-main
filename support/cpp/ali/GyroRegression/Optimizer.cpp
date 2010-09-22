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
#include <cstdlib>
#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"
#include "GyroNLP.hpp"
#include "Optimizer.hpp"
#include "InputData.hpp"
#include "CompileTimeConstants.hpp"

using namespace std;
using namespace Ipopt;

namespace gyro {

Optimizer::Optimizer(const Input& data, std::ostream& os, bool verbose) {

	try {

		SmartPtr<IpoptApplication> app = new IpoptApplication();

		SmartPtr<OptionsList> opt = app->Options();

		opt->SetNumericValue("tol", 1.0e-3);
		opt->SetIntegerValue("print_level", 0);
		opt->SetStringValue("output_file", "ipopt.log");
		opt->SetIntegerValue("file_print_level", 5);
		opt->SetStringValue("hessian_approximation", "limited-memory");
		opt->SetStringValue("limited_memory_update_type", "bfgs");

		ApplicationReturnStatus status(app->Initialize("ipopt.opt"));

		if (status != Solve_Succeeded) {
			cerr << "Error during initialization of IPOPT!" << endl;
			exit(ERROR_INITIALIZATION);
		}

		GyroNLP* const gyro_nlp = new GyroNLP(data, os, verbose);

		gyro_nlp->get_bounds_info(NUMBER_OF_VARIABLES, var_lb_, var_ub_, 0, 0, 0);

		conf_file_id = gyro_nlp->config_file_id();

		status = app->OptimizeTNLP(SmartPtr<TNLP>(gyro_nlp));

		if (status == Invalid_Option) {
			cerr << "Error: invalid option!" << endl;
			exit(ERROR_INVALID_OPTION);
		}
		else if (status == Invalid_Number_Detected) {
			cerr << "Error during function or derivative evaluation!" << endl;
			exit(ERROR_NUMBER_INVALID);
		}
		else if (status != Solve_Succeeded && status != Solved_To_Acceptable_Level) {
			cerr << "Error during optimization, code: " << status << "!" << endl;
			exit(ERROR_OPTIMIZATION);
		}

		g_computed = std::sqrt(-app->Statistics()->FinalObjective());

		g_error = g_computed - (fabs(data.g_ref()));

		const double* x = gyro_nlp->solution();

		for (int i=0; i<NUMBER_OF_VARIABLES; ++i) {
			minimizer[i] = x[i];
		}

	}
	catch(...) {
		cerr << "Unknown error!" << endl;
		exit(ERROR_UNKNOWN);
	}
}

}
