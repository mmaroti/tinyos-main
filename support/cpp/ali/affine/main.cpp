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
#include "affine.hpp"
#include "tnt.h"
//#include "jama_qr.h"

using namespace std;
using namespace TNT;
//using namespace JAMA;

enum {
	CONST,
	X,
	Y,
	SIZE
};

int main() {

	affine<SIZE> a;

	affine<SIZE> b(a);

	a=b;

	a[CONST] = 1.0;
	a[X] = 2.0;
	a[Y] = 3.0;

	b[CONST] = -1.0;
	b[X] = -1.0;
	b[Y] = -2.0;

	for (int i=0; i<1000; ++i) {
		a = ((a+b)-b);
	}

	cout << ((a+b)-b) << endl;

	b = a*b;

	cout << b << endl;

	b[SIZE] = 1.0e-14;

	//=====

	Matrix< affine<SIZE> > A(2,3);

	A[0][0] = a;
	A[0][1] = b;
	A[0][2] = a;

	A[1][0] = b;
	A[1][1] = a;
	A[1][2] = b;

	Matrix< affine<SIZE> > B(A);

	Matrix< affine<SIZE> > C = transpose(B);

	B = A*C;

	cout << "B = A*C: " << endl << B << endl;

	B = A;

	cout << "===========================================" << endl;

	cout << "A: " << endl << A << endl;

	cout << "B: " << endl << B << endl;

	B = A-B;

	cout << "A-B: " << endl << B << endl;

	//==========================================================================

	Array2D< affine<SIZE> > P(3, 3);
	Array2D< affine<SIZE> > q(3, 1);

    //QR< affine<SIZE> > qr(P);
    //Array2D< affine<SIZE> > X = qr.solve(q);

	return 0;
}

