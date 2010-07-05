#include <cassert>
#include <iostream>
#include <cstdlib>
#include "linpol.hpp"

using namespace std;

int linpol::size(0);

string* linpol::vars(0);

void error(const char* msg) {

	cerr << "Error: " << msg << "!" << endl;
	exit(EXIT_FAILURE);
}

void linpol::init_varnames(const string arr[], int length) {

	assert(size == 0);
	assert(length > 0);

	size = length+1;

	vars = new string[size];

	vars[0] = string("");

	for (int i=1; i<size; ++i)
		vars[i] = arr[i-1];
}

void linpol::print_varnames() {

	for (int i=0; i<size; ++i)
		cout << vars[i] << endl;
	cout << endl;
}

linpol::linpol() : val(new double[size]) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] = 0.0;
}

linpol::linpol(bool dummy) : val(new double[size]) {

	assert(size > 1);
}

linpol::linpol(const linpol& other) : val(new double[size]) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] = other.val[i];
}

linpol::linpol(const string& name, double value) : val(new double[size]) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] = 0.0;

	set_coefficient(name, value);
}

linpol& linpol::operator=(const linpol& rhs) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] = rhs.val[i];

	return *this;
}

linpol& linpol::operator+=(const linpol& rhs) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] += rhs.val[i];

	return *this;
}

const linpol operator+(const linpol& lhs, const linpol& rhs) {

	assert(linpol::size > 1);

	linpol result(false);

	for (int i=0; i<linpol::size; ++i)
		result.val[i] = lhs.val[i]+rhs.val[i];

	return result;
}

linpol& linpol::operator-=(const linpol& rhs) {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		val[i] -= rhs.val[i];

	return *this;
}

const linpol operator-(const linpol& lhs, const linpol& rhs) {

	assert(linpol::size > 1);

	linpol result(false);

	for (int i=0; i<linpol::size; ++i)
		result.val[i] = lhs.val[i]-rhs.val[i];

	return result;
}

const linpol operator-(const linpol& pol) {

	assert(linpol::size > 1);

	linpol result(false);

	for (int i=0; i<linpol::size; ++i)
		result.val[i] = - pol.val[i];

	return result;
}

const linpol operator*(const linpol& lhs, const linpol& rhs) {

	assert(linpol::size > 1);

	linpol result(false);

	// c = a*b
	const double* const a = lhs.val;
	const double* const b = rhs.val;

	double* const c = result.val;

	const double a0 = a[0];
	const double b0 = b[0];

	c[0] = a0*b0;

	for (int i=1; i<linpol::size; ++i)
		c[i] = a0*b[i] + b0*a[i];

	return result;
}

linpol::~linpol() {

	delete[] val;
}

int linpol::find_index(const string& name) {

	assert(size > 1);

	for (int i=1; i<size; ++i) {

		if (vars[i] == name)
			return i;
	}

	error("variable name not found, maybe a typo");
	// Control never reaches this line
	return -1;
}

void linpol::set_constant(double value) {

	assert(size > 1);

	val[0] = value;
}

void linpol::set_coefficient(const string& name, double value) {

	assert(size > 1);

	const int index = find_index(name);

	assert ( (0<index) && (index<size) );

	val[index] = value;
}

std::ostream& linpol::print(std::ostream& os) const {

	assert(size > 1);

	for (int i=0; i<size; ++i)
		os << vars[i] << '\t' << val[i] << endl;

	os << endl;

	return os;
}

std::ostream& operator<<(std::ostream& os, const linpol& a) {

	return a.print(os);
}
