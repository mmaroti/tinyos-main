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

void linpol::init_varnames(const string arr[]) {

	assert(size == 0);

	const int n = sizeof(arr)/sizeof(string);

	vars = new string[n];

	size = n;

	for (int i=0; i<n; ++i)
		vars[i] = arr[i];
}

linpol::linpol() : val(new double[size]) {

	assert(size > 0);

	for (int i=0; i<size; ++i)
		val[i] = 0.0;
}

linpol::linpol(const linpol& other) : val(new double[size]) {

	assert(size > 0);

	for (int i=0; i<size; ++i)
		val[i] = other.val[i];
}

linpol& linpol::operator=(const linpol& rhs) {

	assert(size > 0);

	for (int i=0; i<size; ++i)
		val[i] = rhs.val[i];

	return *this;
}

linpol::~linpol() {

	delete[] val;
}

int linpol::find_index(const string& name) {

	assert(size > 0);

	for (int i=0; i<size; ++i) {

		if (vars[i] == name)
			return i;
	}

	error("variable name not found, maybe a typo");
	// Control never reaches this line
	return -1;
}

void linpol::set_coefficient(const string& name, double value) {

	assert(size > 0);

	const int index = find_index(name);

	assert ( (0<=index) && (index<size) );

	val[index] = value;
}




