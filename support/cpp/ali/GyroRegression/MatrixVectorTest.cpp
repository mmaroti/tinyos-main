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
#include "GradType.hpp"
#include "MatrixVector.hpp"

using namespace std;
using namespace gyro;

typedef Vector<double> vector;

void doubleTest() {

	vector a(1.0, 2.0, 0.0);

	vector b(2.0, 1.0, 3.0);

	vector c = a + b;

	cout << c << endl;

	cout << a[X] << endl;

	a = 2*b;
	b = a*1;
	a = a/2;

	cout << a << endl;
	cout << b << endl;

	cout << a*b << endl;

	matrix3 I = matrix3::identity();

	cout << "I*I " << I*I << endl;

	vector3 angles(-M_PI-1.0e-15, M_PI, 0);

	angles.enforce_range_minus_pi_plus_pi();

	cout << "angles " << angles << endl;
}

void gradTypeTest() {

	GradType<5> x(1.0);
	GradType<5> y(1.0);
	GradType<5> z(1.0);

	Vector<GradType<5> > p(x, y, z);

	Vector<GradType<5> > q(2.0, 1.0, 3.0);

	cout << p*q << endl;

	GradType<5> mat[] = { 2.0, 1.0, 3.0, 5.0, 4.0, 8.0, 9.0, 0.0, 3.0 };

	Matrix<GradType<5> > m(mat);

	cout << "mat: " << m << endl;

	cout << "m*p: " << m*p << endl;

	cout << "m*m: " << m*m << endl;

	cout << "m+m" << m+m << endl;

}

int main() {

	doubleTest();

	gradTypeTest();

	return 0;
}

