#include <cassert>
#include <iostream>
#include "linvars.hpp"
#include "linpol.hpp"
#include "utility.hpp"

using namespace std;

linpol::linpol(const linvars* const vars) {

	const int n = vars->size();

	val = new double[n];

	names = vars;

	for (int i=0; i<n; ++i)
		val[i] = 0.0;
}

linpol::linpol(const linpol& other) : val(0), names(0) {

	if (other.names != 0) {

		const int n = other.names->size();

		val = new double[n];

		names = other.names;

		for (int i=0; i<n; ++i)
			val[i] = other.val[i];
	}
}

void linpol::init_if_needed(const linpol& other) {

	assert(other.names != 0);

	if (names==0) {

		const int n = other.names->size();

		val = new double[n];

		names = other.names;

		for (int i=0; i<n; ++i)
			val[i] = 0.0;
	}

	assert(names==other.names);
}

linpol& linpol::operator=(const linpol& rhs) {

	if (rhs.names != 0) {

		init_if_needed(rhs);

		const int n = rhs.names->size();

		for (int i=0; i<n; ++i)
			val[i] = rhs.val[i];
	}

	return *this;
}

// TODO Automatic type conversion?
linpol& linpol::operator=(double rhs) {

	if (rhs != 0.0)
		error("operator= works only with rhs = 0");

	if (val !=0 ) {

		const int n = names->size();

		for (int i=0; i<n; ++i)
			val[i] = 0.0;
	}

	return *this;
}

linpol& linpol::operator+=(const linpol& rhs) {

	init_if_needed(rhs);

	const int n = rhs.names->size();

	for (int i=0; i<n; ++i)
		val[i] += rhs.val[i];

	return *this;
}

const linpol operator+(const linpol& lhs, const linpol& rhs) {

	linpol result(lhs);

	result += rhs;

	return result;
}

linpol& linpol::operator-=(const linpol& rhs) {

	init_if_needed(rhs);

	const int n = rhs.names->size();

	for (int i=0; i<n; ++i)
		val[i] -= rhs.val[i];

	return *this;
}

const linpol operator-(const linpol& lhs, const linpol& rhs) {

	linpol result(lhs);

	result -= rhs;

	return result;
}

const linpol operator-(const linpol& pol) {

	assert(pol.names!=0);

	linpol result(pol.names);

	const int n = pol.names->size();

	for (int i=0; i<n; ++i)
		result.val[i] = - pol.val[i];

	return result;
}

const linpol operator*(const linpol& lhs, const linpol& rhs) {

	assert ( (lhs.names != 0) && (rhs.names != 0));

	linpol result(lhs.names);

	// c = a*b
	const double* const a = lhs.val;
	const double* const b = rhs.val;

	double* const c = result.val;

	const double a0 = a[0];
	const double b0 = b[0];

	c[0] = a0*b0;

	const int n = result.names->size();

	for (int i=1; i<n; ++i)
		c[i] = a0*b[i] + b0*a[i];

	return result;
}

linpol::~linpol() {

	delete[] val;
}

linpol& linpol::set_constant(double value) {

	assert( names!=0 );

	val[0] = value;

	return *this;
}

linpol& linpol::set_coefficient(const string& name, double value) {

	assert( names!=0 );

	const int index = names->find_index(name);

	const int size = names->size();

	assert ( (0<index) && (index<size) );

	val[index] = value;

	return *this;
}

std::ostream& linpol::print(std::ostream& os) const {

	if (names==0) {

		os << "not initialized" << endl;
	}
	else {

		const int size = names->size();

		const string* const vars = names->varnames();

		for (int i=0; i<size; ++i)
			os << vars[i] << '\t' << val[i] << endl;
	}

	os << endl;

	return os;
}

std::ostream& operator<<(std::ostream& os, const linpol& a) {

	return a.print(os);
}
