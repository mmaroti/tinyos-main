#include <iostream>
#include <cassert>
#include "affine.hpp"
#include "interval.hpp"

using namespace std;

affine::affine() {

	for (int i=0; i<=SIZE; ++i)
		val[i] = 0.0;
}

affine::affine(const affine& other) {

	for (int i=0; i<=SIZE; ++i)
		val[i] = other.val[i];
}

affine& affine::operator=(const affine& rhs) {

	for (int i=0; i<=SIZE; ++i)
		val[i] = rhs.val[i];

	return *this;
}

double affine::operator[](int i) const {

	assert( (0<=i) && (i<=SIZE) );

	return val[i];
}

double& affine::operator[](int i) {

	assert( (0<=i) && (i<=SIZE) );

	return val[i];
}

std::ostream& affine::print(std::ostream& os) const {

	for (int i=0; i<=SIZE; ++i)
		os << i << ":\t" << val[i] << endl;

	os << endl;

	return os;
}

const affine operator+(const affine& lhs, const affine& rhs) {

	affine result;

	const double* const x = lhs.val;
	const double* const y = rhs.val;
	      double* const z = result.val;

	double d(0.0);

	for (int i=0; i<SIZE; ++i) {

		double z_i = x[i]+y[i];

		z[i] = z_i;

		double a = q_pred(z_i);
		double b = q_succ(z_i);

		d = q_succ(d + q_succ(max(b-z_i, z_i-a)));
	}

	z[SIZE] = d;

	return result;
}

const affine operator-(const affine& lhs, const affine& rhs) {

	affine result;

	const double* const x = lhs.val;
	const double* const y = rhs.val;
	      double* const z = result.val;

	double d(0.0);

	for (int i=0; i<SIZE; ++i) {

		double z_i = x[i]-y[i];

		z[i] = z_i;

		double a = q_pred(z_i);
		double b = q_succ(z_i);

		d = q_succ(d + q_succ(max(b-z_i, z_i-a)));
	}

	z[SIZE] = d;

	return result;
}

std::ostream& operator<<(std::ostream& os, const affine& a) {

	return a.print(os);
}
