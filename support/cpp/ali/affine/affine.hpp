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
const affine<SIZE> operator*(const affine<SIZE>& lhs, const affine<SIZE>& rhs) {

	affine<SIZE> result;

	const double* const x = lhs.val;
	const double* const y = rhs.val;
	      double* const z = result.val;

	double sx(0.0);
	double sy(0.0);

	for (int i=1; i<SIZE; ++i) {

		sx = q_succ(sx + std::fabs(x[i]));
		sy = q_succ(sy + std::fabs(y[i]));
	}

	double s1(0.0);
	double s2(0.0);

	double e1(0.0);
	double e2(0.0);

	for (int i=1; i<SIZE; ++i) {

		double xy = x[i]*y[i];

		double xy_L = q_pred(xy);
		double xy_U = q_succ(xy);

		if (xy_L > 0.0) {

			double a = q_pred(s1 + xy_L);
			double b = q_succ(s1 + xy_U);
			s1 += xy;
			e1 = q_succ(e1+q_succ(max(b-s1,s1-a)));
		}
		else if (xy_U < 0.0) {

			double a = q_pred(s2 + xy_L);
			double b = q_succ(s2 + xy_U);
			s2 += xy;
			e2 = q_succ(e2+q_succ(max(b-s2,s2-a)));
		}
		// else ???

	}

	for (int i=1; i<SIZE; ++i) {

	}

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
std::ostream& operator<<(std::ostream& os, const affine<SIZE>& a) {

	return a.print(os);
}

#endif
