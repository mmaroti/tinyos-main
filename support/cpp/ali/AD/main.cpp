#include <iostream>

template <int N>
class HessType {

public:

	HessType() { }
	explicit HessType(double constant);
	HessType(const HessType& other);
	HessType& operator=(const HessType& rhs);
	HessType& operator=(double rhs);

	template <int N_VAR>
	friend void init_vars(HessType<N_VAR> var[N_VAR], const double* const x);

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

	var[0].f = x[0];
}

int main() {

	const int N_VARS = 2;
	double v[N_VARS];

	HessType<N_VARS> a;
	HessType<N_VARS> b(1.0);
	HessType<N_VARS> c(a);
	a = c;
	const double three = 3.0;
	c = three;

	HessType<N_VARS> vars[N_VARS];
	init_vars(vars, v);

	return 0;
}

