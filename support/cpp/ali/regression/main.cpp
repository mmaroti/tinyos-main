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

#include "IpIpoptApplication.hpp"
#include "IpSolveStatistics.hpp"
#include "ADOL-C_NLP.hpp"

using namespace Ipopt;

int main(int argc, char* argv[])
{

	if (argc<2) {
		return 127;
	}

	init(argv[1]);

	if (argc==3) {
		dump_Ri_using_hardcoded();
		return 0;
	}

	SmartPtr<TNLP> myadolc_nlp = new MyADOLC_NLP();

	SmartPtr<IpoptApplication> app = new IpoptApplication();

	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded) {
		printf("\n\n*** Error during initialization!\n");
		return (int) status;
	}

	status = app->OptimizeTNLP(myadolc_nlp);

	if (status == Solve_Succeeded) {
		Index iter_count = app->Statistics()->IterationCount();
		printf("\n\n*** The problem solved in %d iterations!\n", iter_count);

		Number final_obj = app->Statistics()->FinalObjective();
		printf("\n\n*** The final value of the objective function is %e.\n", final_obj);
	}


	if (argc==4) {
		dump_Ri();
	}

	return (int) status;
}
