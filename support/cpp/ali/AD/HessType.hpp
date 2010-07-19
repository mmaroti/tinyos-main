#ifndef HESSTYPE_HPP_
#define HESSTYPE_HPP_

#include <ostream>

template <int N>
class HessType {

public:

	HessType() { }

	HessType(double constant);

	HessType(const HessType& other);

	HessType& operator=(const HessType& rhs);

	HessType& operator=(double rhs);

	template <int N_VAR>
	friend void init_vars(HessType<N_VAR> var[N_VAR], const double* const x);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const double y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const double x, const HessType<N_VAR>& y);

	template <int N_VAR>
	friend const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const double y);

	std::ostream& print(std::ostream& os) const;

private:

	void copy(const HessType& other);
	void init(double value);

	double f;
	double g[N];
	double h[N][N];
};

template <int N>
void HessType<N>::init(double value) {
	f = value;
	for (int i=0; i<N; ++i) {
		g[i] = 0.0;
		for (int j=0; j<=i; ++j) {
			h[i][j] = 0.0;
		}
	}
}

template <int N>
void HessType<N>::copy(const HessType& other) {
	f = other.f;
	for (int i=0; i<N; ++i) {
		g[i] = other.g[i];
		for (int j=0; j<=i; ++j) {
			h[i][j] = other.h[i][j];
		}
	}
}

template <int N>
HessType<N>::HessType(double constant) {
	init(constant);
}

template <int N>
HessType<N>::HessType(const HessType& other) {
	copy(other);
}

template <int N>
HessType<N>& HessType<N>::operator=(const HessType<N>& rhs) {
	copy(rhs);
	return *this;
}

template <int N>
HessType<N>& HessType<N>::operator=(double rhs) {
	init(rhs);
	return *this;
}

template <int N_VAR>
void init_vars(HessType<N_VAR> var[N_VAR], const double* x) {

	for (int i=0; i<N_VAR; ++i) {
		var[i] = x[i];
		var[i].g[i] = 1.0;
	}
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x) {

	HessType<N_VAR> z;

	z.f = -x.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = -x.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = -x.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f + y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] + y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x.h[i][j] + y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z(y);

	z.f += x;

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator+(const HessType<N_VAR>& x, const double y) {

	return y+x;
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f - y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] - y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x.h[i][j] - y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator-(const double x, const HessType<N_VAR>& y) {

	return x+(-y);
}

template <int N_VAR>
const HessType<N_VAR> operator-(const HessType<N_VAR>& x, const double y) {

	return x+(-y);
}

template <int N_VAR>
const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = y.f*x.g[i] + x.f*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = y.f*x.h[i][j]+x.g[i]*y.g[j]+y.g[i]*x.g[j]+x.f*y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator*(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = x*y.h[i][j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator*(const HessType<N_VAR>& x, const double y) {

	return y*x;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = (x.g[i] - z.f*y.g[i]) / y.f;
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = (x.h[i][j]-z.g[i]*y.g[j]-y.g[i]*z.g[j]-z.f*y.h[i][j])/y.f;
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const double x, const HessType<N_VAR>& y) {

	HessType<N_VAR> z;

	z.f = x/y.f;
	const double p = -z.f/y.f;
	const double q = (-2.0*p)/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = p*y.g[i];
		for (int j=0; j<=i; ++j) {
			z.h[i][j] = p*y.h[i][j] + q*y.g[i]*y.g[j];
		}
	}

	return z;
}

template <int N_VAR>
const HessType<N_VAR> operator/(const HessType<N_VAR>& x, const double y) {

	return x*(1.0/y);
}

template <int N_VAR>
std::ostream& HessType<N_VAR>::print(std::ostream& os) const {
	os << this->f << std::flush;
	return os;
}

template <int N_VAR>
std::ostream& operator<<(std::ostream& os, const HessType<N_VAR>& x) {
	return x.print(os);
}

#endif
