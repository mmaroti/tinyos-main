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

#include <stdexcept>
#include <sstream>
#include <cmath>
#include "EulerAngles.hpp"

using namespace std;

namespace {

const double PI(3.141592653589793);
const double RAD(57.29577951308232);

enum {
	R11, R12, R13,
	R21, R22, R23,
	R31, R32, R33
};

enum {
	ALPHA, BETA, GAMMA
};

}

namespace gyro {

bool both_zero(double p, double q) {

	return (fabs(p)<TOL_DEGEN) && (fabs(q)<TOL_DEGEN);
}

void set_r31(double& r31) {

	if (r31>1.0-10.0*TOL_DEGEN) {
		r31 = 1.0;
	}
	else if (r31<-1.0+10.0*TOL_DEGEN) {
		r31 = -1.0;
	}
	else {
		ostringstream os;
		os << "r31 = " << r31 << " but row or col is degenerate";
		throw range_error(os.str());
	}
}

bool col_or_row_degen(const double m[9], double& r31) {

	const bool ret(both_zero(m[R11], m[R21]) || both_zero(m[R32], m[R33]));

	if (ret)
		set_r31(r31);

	return ret;
}

bool one(double& r) {

	const bool ret(r>=1.0-TOL_DEGEN);

	if (ret)
		r = 1.0;

	return ret;
}

bool minus_one(double& r) {

	const bool ret(r<=-1.0+TOL_DEGEN);

	if (ret)
		r = -1.0;

	return ret;
}

bool is_degenerate(const double m[9], double& r) {

	return one(r)||minus_one(r)||col_or_row_degen(m, r);
}

void dbg_degen(double r31) {

	if (r31!=-1.0 && r31!=1.0) {
		ostringstream os;
		os << "This is recognized as a degenerate case but r31 = " << r31;
		throw logic_error(os.str());
	}
}

bool rotmat_to_angles(const double m[9], double angle[3]) {

	double r31 = m[R31];

	bool degenerate = is_degenerate(m, r31);

	double alpha, beta, gamma;

	if (degenerate) {
		dbg_degen(r31);
		alpha = 0.0;
		beta  = r31*90.0;
		gamma = atan2(r31*m[R12], m[R22])*RAD;
	}
	else {
		alpha = atan2(m[R21], m[R11])*RAD;
		// FIXME Sign would be needed!
		// Also, m[] has numerical errors, this complicates things :(
		beta  = atan2(-m[R31], sqrt(pow(m[R32],2)+pow(m[R33],2)))*RAD;
		gamma = atan2(m[R32], m[R33])*RAD;
	}

	angle[ALPHA] = alpha; // -180, 180
	angle[BETA]  = beta;  //  -90,  90 FIXME beta range?
	angle[GAMMA] = gamma; // -180, 180

	return degenerate;
}

}
