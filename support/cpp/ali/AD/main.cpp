#include <iostream>

template <int N>
class Multiplier {

public:

	Multiplier(const double* const val);

	int size() const { return N; }

private:

	Multiplier(const Multiplier& );
	Multiplier& operator=(const Multiplier& );

	double v[N];
};

template <int N>
Multiplier<N>::Multiplier(const double* const val) {
	for (int i=0; i<N; ++i)
		v[i] = val[i];
}

template <int N, const double* const v>
class HessType {

public:

	HessType() { }
	explicit HessType(double constant);
	HessType(const HessType& other);
	HessType& operator=(const HessType& rhs);
	HessType& operator=(double rhs);

private:

	void copy(const HessType& other);
	void copy(double value);

	double f;
	double g[N];
	double h[N];
};

template <int N, const double* const v>
void HessType<N, v>::copy(double value) {
	f = value;
	for (int i=0; i<N; ++i) {
		g[i] = 0.0;
		h[i] = 0.0;
	}
}

template <int N, const double* const v>
void HessType<N, v>::copy(const HessType& other) {
	f = other.f;
	for (int i=0; i<N; ++i) {
		g[i] = other.g[i];
		h[i] = other.h[i];
	}
}

template <int N, const double* const v>
HessType<N, v>::HessType(double constant) {
	copy(constant);
}

template <int N, const double* const v>
HessType<N, v>::HessType(const HessType& other) {
	copy(other);
}

template <int N, const double* const v>
HessType<N, v>& HessType<N, v>::operator=(const HessType<N, v>& rhs) {
	copy(rhs);
	return *this;
}

template <int N, const double* const v>
HessType<N, v>& HessType<N, v>::operator=(double rhs) {
	copy(rhs);
	return *this;
}

const int N_VARS = 12;

class Y
{
};
template<class T, T* pT> class X1
{
};



int main() {

	Y aY;

	X1<Y, &aY> x1;

	double v[N_VARS];
	const double* const array = v;
	Multiplier<12> x(v);
	int n = x.size();
	//HessType<N_VARS, array> a;
	// FIXME Ctorban meg kell adni, maskeppen nem megy...
	//HessType<N_VARS, v> b(1.0);
	//HessType<N_VARS, v> c(a);
	//a = c;
	//const double three = 3.0;
	//c = three;

	return 0;
}

