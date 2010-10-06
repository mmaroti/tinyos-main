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

#include "PathNLP.hpp"

#ifdef USE_ADOLC

#include "PathLength.hpp"
#include "CompileTimeConstants.hpp"
#include "BoundReader.hpp"

#include <adolc.h>

#define tag_f 1
#define tag_g 2
#define tag_L 3

namespace gyro {

const int PathNLP::N_VARS(NUMBER_OF_VARIABLES);
const int PathNLP::N_CONS(0);

class PathDouble {

public:

	PathDouble(const double* rotmat, const Input& data, ostream& os)
		: obj(PathLength<double> (rotmat, data, os))
	{

	}

	double evaluate(const double* x) { return obj.f(x); }

private:

	PathLength<double> obj;
};

class PathAD {

public:

	PathAD(const double* rotmat, const Input& data, ostream& os) :
		obj(PathLength<adouble> (rotmat, data, os))
	{

	}

	adouble evaluate(const adouble* x) { return obj.f(x); }

private:

	PathLength<adouble> obj;

};

PathNLP::PathNLP(const double* rotmat, const Input& data, ostream& os) :
		minimizer(new double[N_VARS]),
		obj(new PathDouble(rotmat, data, os)),
		ad(new  PathAD(rotmat, data, os)),
		config(new BoundReader("accel.conf"))
{

}

PathNLP::~PathNLP(){

	delete[] minimizer;
	delete obj;
	delete ad;
	delete config;
}

bool PathNLP::eval_obj(Index n, const double *x, double& obj_value) {

	obj_value = obj->evaluate(x);

	return true;
}

bool PathNLP::eval_obj(Index n, const adouble *x, adouble& obj_value) {

	obj_value = ad->evaluate(x);

	return true;
}

int PathNLP::config_file_id() const {

	return config->file_id();
}

bool PathNLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	n = N_VARS;

	m = N_CONS;

	// in this example the jacobian is dense. Hence, it contains n*m nonzeros
	nnz_jac_g = n*m;

	// the hessian is also dense and has n*n total nonzeros, but we
	// only need the lower left corner (since it is symmetric)
	nnz_h_lag = n*(n-1)/2+n;

	generate_tapes(n, m);

	// use the C style indexing (0-based)
	index_style = C_STYLE;

	return true;
}

bool PathNLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
		Index m, Number* g_l, Number* g_u)
{
	assert(n==NUMBER_OF_VARIABLES);
	const int N_ELEM(9);

	for (int i=0; i<N_ELEM; ++i) {
		x_l[i] = config->elem_lb();
		x_u[i] = config->elem_ub();
	}

	for (int i=N_ELEM; i<NUMBER_OF_VARIABLES; ++i) {
		x_l[i] = config->offset_lb();
		x_u[i] = config->offset_ub();
	}

	// Set the bounds for the constraints
	for (Index i=0; i<m; i++) {
		g_l[i] = 0;
		g_u[i] = 0;
	}

	return true;
}

bool PathNLP::get_starting_point(Index n, bool init_x, Number* x,
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

//*************************************************************************
//
//
//         Nothing has to be changed below this point !!
//
//
//*************************************************************************


bool PathNLP::eval_f(Index n, const Number* x, bool new_x, Number& obj_value)
{
	eval_obj(n,x,obj_value);

	return true;
}

bool PathNLP::eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f)
{

	gradient(tag_f,n,x,grad_f);

	return true;
}

bool PathNLP::eval_g(Index n, const Number* x, bool new_x, Index m, Number* g)
{

	eval_constraints(n,x,m,g);

	return true;
}

bool PathNLP::eval_jac_g(Index n, const Number* x, bool new_x,
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

		jacobian(tag_g,m,n,x,Jac);

		Index idx = 0;
		for(Index i=0; i<m; i++)
			for(Index j=0; j<n; j++)
				values[idx++] = Jac[i][j];

	}

	return true;
}

bool PathNLP::eval_h(Index n, const Number* x, bool new_x,
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

		for(Index i = 0; i<n ; i++)
			x_lam[i] = x[i];
		for(Index i = 0; i<m ; i++)
			x_lam[n+i] = lambda[i];
		x_lam[n+m] = obj_factor;

		hessian(tag_L,n+m+1,x_lam,Hess);

		Index idx = 0;

		for(Index i = 0; i<n ; i++)
		{
			for(Index j = 0; j<=i ; j++)
			{
				values[idx++] = Hess[i][j];
			}
		}
	}

	return true;
}

void PathNLP::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U,
		Index m, const Number* g, const Number* lambda,
		Number obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq)
{

	for (int i=0; i<n; ++i) {
		minimizer[i] = x[i];
	}

	// Memory deallocation for ADOL-C variables

	delete[] x_lam;

	for(Index i=0;i<m;i++)
		delete[] Jac[i];
	delete[] Jac;

	for(Index i=0;i<n+m+1;i++)
		delete[] Hess[i];
	delete[] Hess;
}


//***************    ADOL-C part ***********************************

void PathNLP::generate_tapes(Index n, Index m)
{
	Number *xp    = new double[n];
	Number *lamp  = new double[m];
	Number *zl    = new double[m];
	Number *zu    = new double[m];

	adouble *xa   = new adouble[n];
	adouble *g    = new adouble[m];
	adouble *lam  = new adouble[m];
	adouble sig;
	adouble obj_value;

	double dummy;

	Jac = new double*[m];
	for(Index i=0;i<m;i++)
		Jac[i] = new double[n];

	x_lam   = new double[n+m+1];

	Hess = new double*[n+m+1];
	for(Index i=0;i<n+m+1;i++)
		Hess[i] = new double[i+1];

	get_starting_point(n, 1, xp, 0, zl, zu, m, 0, lamp);

	trace_on(tag_f);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];

	eval_obj(n,xa,obj_value);

	obj_value >>= dummy;

	trace_off();

	trace_on(tag_g);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];

	eval_constraints(n,xa,m,g);


	for(Index i=0;i<m;i++)
		g[i] >>= dummy;

	trace_off();

	trace_on(tag_L);

	for(Index i=0;i<n;i++)
		xa[i] <<= xp[i];
	for(Index i=0;i<m;i++)
		lam[i] <<= 1.0;
	sig <<= 1.0;

	eval_obj(n,xa,obj_value);

	obj_value *= sig;
	eval_constraints(n,xa,m,g);

	for(Index i=0;i<m;i++)
		obj_value += g[i]*lam[i];

	obj_value >>= dummy;

	trace_off();

	delete[] xa;
	delete[] xp;
	delete[] g;
	delete[] lam;
	delete[] lamp;
	delete[] zu;
	delete[] zl;

}

}

#endif

