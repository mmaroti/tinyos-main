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

#ifndef PATHOPTIMIZER_HPP_
#define PATHOPTIMIZER_HPP_

#include <iostream>
#include "CompileTimeConstants.hpp"

namespace gyro {

class Input;

class PathOptimizer {

public:

	PathOptimizer(const double* rotmat, const Input& data, std::ostream& os = std::cout);

	const double* solution() const { return minimizer; }

	int n_vars() const { return NUMBER_OF_VARIABLES; }

	double path_length() const { return pathlength; }

	int config_file_id() const { return conf_file_id; }

	const double* var_lb() const { return var_lb_; }

	const double* var_ub() const { return var_ub_; }

private:

	double minimizer[NUMBER_OF_VARIABLES];

	double var_lb_[NUMBER_OF_VARIABLES];

	double var_ub_[NUMBER_OF_VARIABLES];

	int conf_file_id;

	double pathlength;

};

}

#endif
