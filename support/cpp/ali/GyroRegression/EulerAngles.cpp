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

enum {
	R11, R12, R13,
	R21, R22, R23,
	R31, R32, R33
};

enum {
	X, Y, Z
};

}

namespace dbg {

void degen(double r31) {

	if ( (r31 != -1.0) && (r31 != 1.0)) {

		ostringstream os;
		os << "This case was recognized as a degenerate case but r31 = " << r31;
		throw runtime_error(os.str());
	}
}

bool in_range(double x, double y, double z) {

	return (-180<=x) && (x<=180) &&
		   ( -90<=y) && (y<= 90) &&
		   (-180<=z) && (z<=180) ;

}

void angles(double x, double y, double z) {

	bool is_in_range = in_range(x, y, z);

	if (!is_in_range) {
		ostringstream os;
		os << "An angle is out of range of: " << x << ", " << y << ", " << z;
		throw runtime_error(os.str());
	}
}

void dotprod(const double matrix[3][3], const char* type, int row1, int row2) {

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

void dotprod(const double matrix[3][3], const char* row_or_col) {

	for (int i=0; i<3; ++i) {

		for (int j=i; j<3; ++j) {

			dotprod(matrix, row_or_col, i, j);
		}
	}
}

void orthogonality(const double m[9]) {

	const double r[3][3] = {	{ m[R11], m[R12], m[R13] },
								{ m[R21], m[R22], m[R23] },
								{ m[R31], m[R32], m[R33] }
							};

	const double c[3][3] = {	{ m[R11], m[R21], m[R31] },
								{ m[R12], m[R22], m[R32] },
								{ m[R13], m[R23], m[R33] }
							};

	dotprod(r, "row");

	dotprod(c, "column");

}

bool equals(double x, double y) {

	const double TOL_EQUAL(1.0e-5);

	return fabs(x-y)<TOL_EQUAL;
}

bool equal_360(double diff, int len) {

	if (len==3)
		return equals(fabs(diff), 360);

	return false;
}

void equal_arrays(const double a[], const double b[], int len, const char* name) {

	for (int i=0; i<len; ++i) {

		if (!equals(a[i], b[i]) && !equal_360(a[i]-b[i], len)) {

			ostringstream os;
			os << name << " element mismatch: " << a[i] << " != " << b[i];
			throw runtime_error(os.str());
		}
	}
}

void equal_rotmat(const double m1[9], const double m2[9]) {

	equal_arrays(m1, m2, 9, "Matrix");
}

void enforce_range_x(double& x) {

	if (x > 180) {
		x-= 360;
	}
	else if (x <= -180) {
		x += 360;
	}
}

void equal_degenerate_angles(const double in[3], const double computed[3]) {

	double input[3];

	input[Y] = in[Y];

	input[Z] = 0;

	if (equals(in[Y], 90)) {

		input[X] = in[X]-in[Z];
	}
	else if (equals(in[Y], -90)) {

		input[X] = in[X]+in[Z];
	}
	else {

		throw logic_error("Well, this is a bug...");
	}

	enforce_range_x(input[X]);

	equal_arrays(input, computed, 3, "Angles array");
}

void enforce_range(double angle[3]) {

	if (angle[X] == -180)
		angle[X] = 180;

	if (angle[Z] == -180)
		angle[Z] = 180;
}

void equal_angles(const double in[3], const double computed[3], bool degenerate) {

	bool input_in_range = in_range(in[X], in[Y], in[Z]);

	if (input_in_range) {

		double input[] = { in[X], in[Y], in[Z] };

		enforce_range(input);

		if (!degenerate) {

			equal_arrays(input, computed, 3, "Angles array");
		}
		else {

			equal_degenerate_angles(input, computed);
		}
	}

}

void consistent(const double angles_deg[3]) {

	using namespace gyro;

	double m1[9];

	angles_to_rotmat(angles_deg, m1);

	orthogonality(m1);

	double xyz[3];

	bool degenerate = rotmat_to_angles(m1, xyz);

	double m2[9];

	angles_to_rotmat(xyz, m2);

	orthogonality(m2);

	equal_rotmat(m1, m2);

	equal_angles(angles_deg, xyz, degenerate);

}

}  // namespace dbg

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
		os << "r31 = " << r31 << " but row or column is degenerate";
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

void enforce_range(double& x, double& z) {

	if (x==-180)
		x = 180;

	if (z==-180)
		z = 180;
}

//==============================================================================
//
// Based on http://www.gregslabaugh.name/publications/euler.pdf
//
// Right-handed coordinate system
//
bool rotmat_to_angles(const double m[9], double angle[3]) {

	dbg::orthogonality(m);

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
		dbg::degen(r31);
		y = -r31*90.0;
		z = 0.0;
		x = atan2(-r31*m[R12], m[R22])*RAD; // TODO Explain sign, is handedness correct?
	}

	enforce_range(x, z);

	dbg::angles(x, y, z);

	angle[X] = x; // (-180, 180]
	angle[Y] = y; // [ -90,  90]
	angle[Z] = z; // (-180, 180]

	return degenerate;
}

void angles_to_rotmat(const double angle[3], double m[9]) {

	const double x = angle[X]/RAD;
	const double y = angle[Y]/RAD;
	const double z = angle[Z]/RAD;

	const double sin_x = sin(x);
	const double sin_y = sin(y);
	const double sin_z = sin(z);

	const double cos_x = cos(x);
	const double cos_y = cos(y);
	const double cos_z = cos(z);

	m[R11] = cos_y*cos_z;
	m[R12] = cos_z*sin_x*sin_y-cos_x*sin_z;
	m[R13] = sin_x*sin_z+cos_x*cos_z*sin_y;

	m[R21] = cos_y*sin_z;
	m[R22] = sin_x*sin_y*sin_z+cos_x*cos_z;
	m[R23] = cos_x*sin_y*sin_z-cos_z*sin_x;

	m[R31] = -sin_y;
	m[R32] = cos_y*sin_x;
	m[R33] = cos_x*cos_y;

}

void rotate_vector(const double m[9], const double v[3], double result[3]) {

	dbg::orthogonality(m);

	result[X] = m[R11]*v[X]+m[R12]*v[Y]+m[R13]*v[Z];
	result[Y] = m[R21]*v[X]+m[R22]*v[Y]+m[R23]*v[Z];
	result[Z] = m[R31]*v[X]+m[R32]*v[Y]+m[R33]*v[Z];
}

void inverse_rot_vector(const double m[9], const double v[3], double result[3]) {

	dbg::orthogonality(m);

	result[X] = m[R11]*v[X]+m[R21]*v[Y]+m[R31]*v[Z];
	result[Y] = m[R12]*v[X]+m[R22]*v[Y]+m[R23]*v[Z];
	result[Z] = m[R13]*v[X]+m[R23]*v[Y]+m[R33]*v[Z];
}

void rotmat_to_asin_angles(const double m[9], double angle_deg[3]) {

	dbg::orthogonality(m);

	double r[] = { m[R31], m[R32], m[R33] };

	for (int i=0; i<3; ++i) {

		if (r[i] < -1) {
			r[i] = -1;
		}
		else if (r[i] > 1) {
			r[i] = 1;
		}

		angle_deg[i] = asin(r[i])*RAD;
	}
}

}
