#ifndef GRADTYPE_HPP_
#define GRADTYPE_HPP_

#include <iosfwd>

namespace gyro {

template <int N>
class GradType {

public:

	GradType() { }

	GradType(double constant) { init(constant); }

	GradType(const GradType& other) { copy(other); }

	GradType& operator=(const GradType& rhs) { copy(rhs); return *this; }

	GradType& operator=(double rhs) { init(rhs); return *this; }

	template <int N_VAR>
	friend void init_vars(GradType<N_VAR> var[N_VAR], const double* const x);

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x);

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const double x, const GradType<N_VAR>& y)
	{
		GradType<N_VAR> z(y); z.f += x; return z;
	}

	template <int N_VAR>
	friend const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const double y) { return y+x; }

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const double x, const GradType<N_VAR>& y) { return x+(-y); }

	template <int N_VAR>
	friend const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const double y) { return x+(-y); }

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const double x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const double y) { return y*x; }

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const double x, const GradType<N_VAR>& y);

	template <int N_VAR>
	friend const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const double y) { return x*(1.0/y); }

	double value() const { return f; }

	const double* gradient() const { return g; }

	int size() const { return N; }

	std::ostream& print(std::ostream& os) const { os << this->f; return os; }

private:

	void copy(const GradType& other);
	void init(double value);

	double f;
	double g[N];
};

template <int N_VAR>
std::ostream& operator<<(std::ostream& os, const GradType<N_VAR>& x) {
	return x.print(os);
}

template <int N>
void GradType<N>::init(double value) {
	f = value;
	for (int i=0; i<N; ++i) {
		g[i] = 0.0;
	}
}

template <int N>
void GradType<N>::copy(const GradType& other) {
	f = other.f;
	for (int i=0; i<N; ++i) {
		g[i] = other.g[i];
	}
}

template <int N_VAR>
void init_vars(GradType<N_VAR> var[N_VAR], const double* x) {

	for (int i=0; i<N_VAR; ++i) {
		var[i] = x[i];
		var[i].g[i] = 1.0;
	}
}

template <int N_VAR>
const GradType<N_VAR> operator-(const GradType<N_VAR>& x) {

	GradType<N_VAR> z;

	z.f = -x.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = -x.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator+(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f + y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] + y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator-(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f - y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x.g[i] - y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator*(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = y.f*x.g[i] + x.f*y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator*(const double x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x*y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = x*y.g[i];
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator/(const GradType<N_VAR>& x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = (x.g[i] - z.f*y.g[i]) / y.f;
	}

	return z;
}

template <int N_VAR>
const GradType<N_VAR> operator/(const double x, const GradType<N_VAR>& y) {

	GradType<N_VAR> z;

	z.f = x/y.f;
	const double p = -z.f/y.f;
	for (int i=0; i<N_VAR; ++i) {
		z.g[i] = p*y.g[i];
	}

	return z;
}

}

#endif
