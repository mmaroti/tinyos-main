#include <iostream>
#include "linpol.hpp"

using namespace std;

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

	return 0;
}
