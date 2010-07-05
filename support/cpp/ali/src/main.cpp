#include <iostream>
#include "linpol.hpp"
#include "tnt.h"

using namespace std;
using namespace TNT;

int main() {

	string arr[] = { "x1", "x2", "x3" };

	linpol::init_varnames(arr, sizeof(arr)/sizeof(string));

	linpol a;

	linpol::print_varnames();

	linpol b(a);

	b.set_coefficient("x1", 1.0);

	a = b;

	linpol c("x2", 2.0);

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

	Matrix<linpol> A(2,3);

	A[0][0] = linpol("x1", 2.0);
	A[0][1] = linpol("x2", 3.0);
	A[0][2] = linpol("x3", 4.0);

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
