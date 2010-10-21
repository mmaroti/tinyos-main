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

#ifdef USE_GRADTYPE

#include <iostream>
#include "GradType.hpp"
#include "CompileTimeConstants.hpp"
#include "Objective.hpp"
#include "RegressionNLP_GradType.hpp"
#include "BoundReader.hpp"

namespace {

	const int NUM_OF_VARS = 24;
}

namespace gyro {

const int RegressionNLP::N_VARS(NUM_OF_VARS);
const int RegressionNLP::N_CONS(0);

class RegressionDouble {

public:

	RegressionDouble(const Input& data, std::ostream& os) :
		obj(Objective<double> (data, os, false))
	{

	}

	double evaluate(const double* x) { return obj.f(x); }

private:

	Objective<double> obj;
};

class RegressionGrad {

public:

	RegressionGrad(const Input& data, std::ostream& os) :
		obj(Objective<GradType<NUM_OF_VARS> > (data, os, false))
	{

	}

	void evaluate(const double* x, double* grad_f) {

		GradType<NUM_OF_VARS> vars[NUM_OF_VARS];

		init_vars(vars, x);

		const GradType<NUM_OF_VARS> result = obj.f(vars);

		const double* const g = result.gradient();

		for (int i=0; i<NUM_OF_VARS; ++i)
			grad_f[i] = g[i];
	}

private:

	Objective<GradType<NUM_OF_VARS> > obj;
};

RegressionNLP::RegressionNLP(const Input& data, std::ostream& os) :
		minimizer(new double[N_VARS]),
		obj(new RegressionDouble(data, os)),
		grad(new  RegressionGrad(data, os)),
		config(new BoundReader("accel.txt"))
{

}

RegressionNLP::~RegressionNLP(){

	delete[] minimizer;
	delete obj;
	delete grad;
	delete config;
}

bool RegressionNLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
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

bool RegressionNLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
		Index m, Number* g_l, Number* g_u)
{
	assert(n==NUM_OF_VARS);

	// FIXME Either hard-coded or 2 bound readers

	for (int i=0; i<NUM_OF_VARS; ++i) {
		x_l[i] = -0.001;
		x_u[i] =  0.001;
	}

	for (int i=9; i<12; ++i) {
		x_l[i] = -0.1;
		x_u[i] =  0.1;
	}

	for (int i=21; i<24; ++i) {
		x_l[i] = -0.1;
		x_u[i] =  0.1;
	}

	// Set the bounds for the constraints
	for (Index i=0; i<m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}

	return true;
}

bool RegressionNLP::get_starting_point(Index n, bool init_x, Number* x,
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

void RegressionNLP::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U,
		Index m, const Number* g, const Number* lambda,
		Number obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq)
{
	for (int i=0; i<n; ++i) {
		minimizer[i] = x[i];
	}
}

int RegressionNLP::config_file_id() const {

	return config->file_id();
}

bool RegressionNLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
	// TODO new_x -> caching?
	obj_value = obj->evaluate(x);
	return true;
}

bool RegressionNLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{
	// TODO new_x -> caching?
	grad->evaluate(x, grad_f);
	return true;
}

bool RegressionNLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{
	// This problem does not have constraints
	assert(m == 0);
	return true;
}

bool RegressionNLP::eval_jac_g(Index n, const Number* x, bool new_x,
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

}

#endif
