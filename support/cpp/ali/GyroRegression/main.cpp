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
#include "CompileTimeConstants.hpp"
#include "Optimizer.hpp"
#include "PathOptimizer.hpp"
#include "DataIO.hpp"
#include "RotationMatrix.hpp"
#include "InputData.hpp"

using namespace std;

using namespace gyro;

void run_solver(const Input& data, const char* outfile) {

	Optimizer opt(data);

	const double* const x = opt.solution();

	RotationMatrix rot(data, x);

	write_result(outfile, opt, data, rot);

	PathOptimizer path(rot.get_rot_matrices(), data);
}

int main(int argc, char* argv[]) {

	if (argc!=3) {
		cerr << "Usage: " << argv[0] << " input_file_name  output_file_name" << endl;
		return ERROR_ARG_COUNT;
	}

	const Input* data = read_file(argv[1]);

	run_solver(*data, argv[2]);

	delete data;

	return SUCCESS;

}
