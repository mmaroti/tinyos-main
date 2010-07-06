#include <iostream>
#include "affine.hpp"

using namespace std;

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

	return 0;
}

