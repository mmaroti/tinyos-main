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
#include "linpol.hpp"
#include "linvars.hpp"
#include "tnt.h"

using namespace std;
using namespace TNT;

int main() {

	string arr[] = { "x1", "x2", "x3" };

	linvars names(arr, sizeof(arr)/sizeof(string));

	linpol a(&names);

	cout << names << endl;

	linpol b(a);

	b.set_coefficient("x1", 1.0);

	a = b;

	linpol c;

	a = b = c;

	a.set_constant(1.0);

	const double x = 3.0;

	b.set_coefficient("x3", x);

	c = a+b;

	a+=b;

	b = -a;

	linpol d(a*b*c);

	cout << "a" << endl << a << endl;
	cout << "b" << endl << b << endl;
	cout << "c" << endl << c << endl;
	cout << "d" << endl << d << endl;

	//==========================================================================

	Matrix<linpol> A(2,3,linpol(&names));

	A[0][0] = linpol(&names).set_coefficient("x1", 2.0).set_constant(1.0);
	A[0][1] = linpol(&names).set_coefficient("x2", 3.0).set_constant(5.0);
	A[0][2] = linpol(&names).set_coefficient("x3", 4.0).set_constant(3.0);

	A[1][0] = d;
	A[1][1] = a;
	A[1][2] = c;

	Matrix<linpol> B(A);

	Matrix<linpol> C = transpose(B);

	B = A*C;

	cout << "B: " << endl << B << endl;

	B = A;

	B = A-B;

	cout << "B: " << endl << B << endl;

	return 0;
}
