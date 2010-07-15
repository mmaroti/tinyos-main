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
