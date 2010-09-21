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

#include <assert.h>
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
	X, Y, Z
};

void dbg_degen(double r31) {

	if ( (r31 != -1.0) && (r31 != 1.0)) {

		ostringstream os;
		os << "This case was recognized as a degenerate case but r31 = " << r31;
		throw runtime_error(os.str());
	}
}

void dbg_angles(double x, double y, double z) {

	assert( (-180<=x) && (x<=180) );
	assert( ( -90<=y) && (y<= 90) );
	assert( (-180<=z) && (z<=180) );
}

void dbg_dotprod(const double matrix[3][3], const char* type, int row1, int row2) {

	const double TOL_ORTHO(1.0e-4);

	double dot_prod = 0.0;

	for (int col=0; col<3; ++col) {
		dot_prod += (matrix[row1][col]*matrix[row2][col]);
	}

	if (row1==row2) {
		dot_prod -= 1;
	}

	if (fabs(dot_prod) > TOL_ORTHO) {
		ostringstream os;
		os << type << " (" << (row1+1) << ", " << (row2+1) << ") = ";
		os << dot_prod;
		throw runtime_error(os.str());
	}
}

void dbg_dotprod(const double matrix[3][3], const char* row_or_col) {

	for (int i=0; i<3; ++i) {

		for (int j=i; j<3; ++j) {

			dbg_dotprod(matrix, row_or_col, i, j);
		}
	}

}

void dbg_orthogonality(const double m[9]) {

	const double r[3][3] = {	{ m[R11], m[R12], m[R13] },
								{ m[R21], m[R22], m[R23] },
								{ m[R31], m[R32], m[R33] }
							};

	const double c[3][3] = {	{ m[R11], m[R21], m[R31] },
								{ m[R12], m[R22], m[R32] },
								{ m[R13], m[R23], m[R33] }
							};

	dbg_dotprod(r, "row");
	dbg_dotprod(c, "column");

}

}

namespace gyro {

bool both_zero(double p, double q) {

	return (fabs(p) < TOL_DEGEN) && (fabs(q) < TOL_DEGEN);
}

void set_r31(double& r31) {

	if (r31 > 1.0 - 10.0*TOL_DEGEN) {

		r31 = 1.0;
	}
	else if (r31 < -1.0 + 10.0*TOL_DEGEN) {

		r31 = -1.0;
	}
	else {

		ostringstream os;
		os << "r31 = " << r31 << " but row or col is degenerate";
		throw range_error(os.str());
	}
}

bool col_or_row_degen(const double m[9], double& r31) {

	const bool ret( both_zero(m[R11], m[R21]) || both_zero(m[R32], m[R33]) );

	if (ret) {

		set_r31(r31);
	}

	return ret;
}

bool one(double& r) {

	const bool ret( r >= 1.0 - TOL_DEGEN );

	if (ret) {

		r = 1.0;
	}

	return ret;
}

bool minus_one(double& r) {

	const bool ret( r <= -1.0 + TOL_DEGEN );

	if (ret) {
		r = -1.0;
	}

	return ret;
}

bool is_degenerate(const double m[9], double& r) {

	return one(r) || minus_one(r) || col_or_row_degen(m, r);
}

//
// Based on http://www.gregslabaugh.name/publications/euler.pdf
//
// Right-handed coordinate system
//
bool rotmat_to_angles(const double m[9], double angle[3]) {

	dbg_orthogonality(m);

	double r31 = m[R31];

	bool degenerate = is_degenerate(m, r31);

	double x, y, z;

	if (!degenerate) {
		y = asin(-m[R31])*RAD;
		z = atan2(m[R21], m[R11])*RAD;
		x = atan2(m[R32], m[R33])*RAD;
	}
	else {
		// r31 = +1 or -1
		dbg_degen(r31);
		y = (-r31)*90.0;
		z = 0.0;
		x = atan2(r31*m[R12], m[R22])*RAD;
	}

	dbg_angles(x, y, z);

	angle[X] = x; // -180, 180
	angle[Y] = y; //  -90,  90
	angle[Z] = z; // -180, 180

	return degenerate;
}

}
