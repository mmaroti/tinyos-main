#include <iostream>
#include <cstdlib>
#include "GyroNLP.hpp"
#include "ObjectiveEvaluator.hpp"
#include "GradType.hpp"

using namespace std;

const int NUMBER_OF_VARIABLES(12);

const int GyroNLP::N_VARS(NUMBER_OF_VARIABLES);
const int GyroNLP::N_CONS(0);

class ObjDouble {

public:

	ObjDouble(const input& data, ostream& os) :
		obj(ObjEval<double> (os, data))
	{

	}

	// TODO Implement dump of M*R(i)

private:

	ObjEval<double> obj;
};

class ObjGrad {

public:

	ObjGrad(const input& data, ostream& os) :
		obj(ObjEval<GradType<NUMBER_OF_VARIABLES> > (os, data))
	{

	}

private:

	ObjEval<GradType<NUMBER_OF_VARIABLES> > obj;
};

GyroNLP::GyroNLP(const input& data, ostream& os) :
		out(os),
		solution(new double[N_VARS]),
		obj(new ObjDouble(data, os)),
		grad(new  ObjGrad(data, os))
{

}

GyroNLP::~GyroNLP(){

	delete[] solution;
	delete obj;
	delete grad;
}

bool GyroNLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	n = N_VARS;

	m = N_CONS;

	// The dense Jacobian contains n*m nonzeros
	nnz_jac_g = n*m;

	// The dense Hessian has n*n total nonzeros, only the lower left corner is
	// stored since it is symmetric
	nnz_h_lag = n*(n-1)/2+n;

	// use the C style indexing (0-based)
	index_style = C_STYLE;

	return true;
}

bool GyroNLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
		Index m, Number* g_l, Number* g_u)
{

	for (Index i=0; i<n; i++) {
		x_l[i] = -1.0;
		x_u[i] =  1.0;
	}

	// Set the bounds for the constraints
	for (Index i=0; i<m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}

	return true;
}

bool GyroNLP::get_starting_point(Index n, bool init_x, Number* x,
		bool init_z, Number* z_L, Number* z_U,
		Index m, bool init_lambda,
		Number* lambda)
{
	assert(init_x == true);
	assert(init_z == false);
	assert(init_lambda == false);

	// set the starting point
	for (Index i=0; i<n; i++)
		x[i] = 0.0;

	return true;
}

void GyroNLP::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U,
		Index m, const Number* g, const Number* lambda,
		Number obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq)
{
	for (int i=0; i<n; ++i) {
		solution[i] = x[i];
	}
}


bool GyroNLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
	// FIXME
	//eval_obj(n,x,obj_value);

	return true;
}

bool GyroNLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
	// FIXME
	//gradient(tag_f,n,x,grad_f);

	return true;
}

bool GyroNLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
	// This problem does not have constraints
	assert(m == 0);
	return true;
}

bool GyroNLP::eval_jac_g(Index n, const Number* x, bool new_x,
		Index m, Index nele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	if (values == NULL) {
		// return the structure of the jacobian,
		// assuming that the Jacobian is dense

		Index idx = 0;
		for(Index i=0; i<m; i++)
			for(Index j=0; j<n; j++)
			{
				iRow[idx] = i;
				jCol[idx++] = j;
			}
	}
	else {
		// return the values of the jacobian of the constraints

		// This problem does not have constraints
		assert(m == 0);
	}

	return true;
}

bool GyroNLP::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda,
		bool new_lambda, Index nele_hess, Index* iRow,
		Index* jCol, Number* values)
{
	if (values == NULL) {
		// return the structure. This is a symmetric matrix, fill the lower left
		// triangle only.

		// the hessian for this problem is actually dense
		Index idx=0;
		for (Index row = 0; row < n; row++) {
			for (Index col = 0; col <= row; col++) {
				iRow[idx] = row;
				jCol[idx] = col;
				idx++;
			}
		}

		assert(idx == nele_hess);
	}
	else {
		// return the values. This is a symmetric matrix, fill the lower left
		// triangle only

		cerr << "Error: evaluation of the Hessian is not implemented!" << endl;
		cerr << "Use the following option in the ipopt.opt configuration file:" << endl;
		cerr << "hessian_approximation limited-memory" << endl;
		exit(EXIT_FAILURE);
	}

	return true;
}

