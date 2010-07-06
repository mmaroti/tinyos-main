#include <iostream>
#include "affine.hpp"

using namespace std;

int main() {

	affine a;

	affine b(a);

	a=b;

	a[CONST] = 1.0;
	a[X] = 2.0;
	a[Y] = 3.0;

	b[CONST] = -1.0;
	b[X] = -1.0;
	b[Y] = -2.0;

	cout << ((a+b)-b) << endl;

	return 0;
}

