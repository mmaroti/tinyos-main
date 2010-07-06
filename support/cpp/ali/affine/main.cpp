#include <iostream>
#include "affine.hpp"
#include "tnt.h"

using namespace std;
using namespace TNT;

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

	B = A-B;

	cout << "B: " << endl << B << endl;

	return 0;
}

