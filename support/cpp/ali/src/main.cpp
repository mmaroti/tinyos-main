#include <iostream>
#include "linpol.hpp"

using namespace std;

int main() {

	string arr[] = { "x0", "y1" };

	linpol::init_varnames(arr);

	linpol a;

	linpol b(a);

	b.set_coefficient("x0", 1.0);

	a = b;

	linpol c("y1", 2.0);

	c = a = b;

	return 0;
}
