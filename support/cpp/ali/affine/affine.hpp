#ifndef AFFINE_HPP_
#define AFFINE_HPP_

#include <iosfwd>
#include <cmath>
#include <cassert>

extern "C" {
	double q_pred(double);
	double q_succ(double);
}

template <int SIZE>
class affine {

public:

	affine();

	affine(const affine& other);

	affine& operator=(const affine& rhs);

	// Out of necessity, only works with rhs = 0
	affine& operator=(double rhs);

	template <int N>
	friend const affine<N> operator+(const affine<N>& lhs, const affine<N>& rhs);

	template <int N>
	friend const affine<N> operator-(const affine<N>& lhs, const affine<N>& rhs);

	template <int N>
	friend const affine<N> operator*(const affine<N>& lhs, const affine<N>& rhs);

	double  operator[](int i) const;

	double& operator[](int i);

	std::ostream& print(std::ostream& os) const;

private:

	double val[SIZE+1];
};

using namespace std;

template <int SIZE>
affine<SIZE>::affine() {

	for (int i=0; i<=SIZE; ++i)
		val[i] = 0.0;
}

template <int SIZE>
affine<SIZE>::affine(const affine<SIZE>& other) {

	for (int i=0; i<=SIZE; ++i)
		val[i] = other.val[i];
}

template <int SIZE>
affine<SIZE>& affine<SIZE>::operator=(const affine<SIZE>& rhs) {

	for (int i=0; i<=SIZE; ++i)
		val[i] = rhs.val[i];

	return *this;
}

template <int SIZE>
affine<SIZE>& affine<SIZE>::operator=(double rhs) {

	assert (rhs == 0);

	for (int i=0; i<=SIZE; ++i)
		val[i] = 0.0;

	return *this;
}

template <int SIZE>
double affine<SIZE>::operator[](int i) const {

	assert( (0<=i) && (i<=SIZE) );

	return val[i];
}

template <int SIZE>
double& affine<SIZE>::operator[](int i) {

	assert( (0<=i) && (i<=SIZE) );

	return val[i];
}

template <int SIZE>
std::ostream& affine<SIZE>::print(std::ostream& os) const {

	for (int i=0; i<=SIZE; ++i)
		os << i << ":\t" << val[i] << endl;

	os << endl;

	return os;
}

template <int SIZE>
const affine<SIZE> operator+(const affine<SIZE>& lhs, const affine<SIZE>& rhs) {

	affine<SIZE> result;

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

	d = q_succ(d + x[SIZE]);
	d = q_succ(d + y[SIZE]);

	z[SIZE] = d;

	return result;
}

template <int SIZE>
const affine<SIZE> operator-(const affine<SIZE>& lhs, const affine<SIZE>& rhs) {

	affine<SIZE> result;

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

	d = q_succ(d + x[SIZE]);
	d = q_succ(d + y[SIZE]);

	z[SIZE] = d;

	return result;
}

template <int SIZE>
const affine<SIZE> operator*(const affine<SIZE>& lhs, const affine<SIZE>& rhs) {

	using std::fabs;

	affine<SIZE> result;

	const double* const x = lhs.val;
	const double* const y = rhs.val;

	double rx(fabs(x[1]));
	double ry(fabs(y[1]));

	for (int i=2; i<=SIZE; ++i) {

		rx = q_succ(rx + fabs(x[i]));
		ry = q_succ(ry + fabs(y[i]));
	}

	double dr = q_succ(rx*ry);

	const double x0 = x[0];
	const double y0 = y[0];

	double z0 = x0*y0;

	double z0_pre = q_pred(z0);
	double z0_suc = q_succ(z0);

	double d = q_succ(max(z0_suc-z0, z0-z0_pre));

	double* const z = result.val;

	z[0] = z0;

	for (int i=1; i<SIZE; ++i) {

		double xy0 = y0*x[i];
		double yx0 = x0*y[i];

		double zL = q_pred(q_pred(xy0)+q_pred(yx0));
		double zU = q_succ(q_succ(xy0)+q_succ(yx0));

		double zi = xy0+yx0;

		z[i] = zi;

		d = q_succ(d+q_succ(max(zU-zi, zi-zL)));
	}

	d = q_succ(d+dr);

	z[SIZE] = d;

	return result;
}

template <int SIZE>
std::ostream& operator<<(std::ostream& os, const affine<SIZE>& a) {

	return a.print(os);
}

#endif
