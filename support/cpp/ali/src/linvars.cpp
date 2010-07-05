#include <iostream>
#include "linvars.hpp"
#include "utility.hpp"

using namespace std;

linvars::linvars(const string arr[], int length) {

	if (length < 1)
		error("there should be at least one variable");

	n = length+1;

	vars = new string[n];

	vars[0] = string("");

	for (int i=1; i<n; ++i)
		vars[i] = arr[i-1];

}

linvars::~linvars() {

	delete[] vars;
}

int linvars::size() const { return n; }

int linvars::find_index(const string & name) const {

	for (int i=1; i<n; ++i) {

		if (vars[i] == name)
			return i;
	}

	error("variable name not found, maybe a typo");
	// Control never reaches this line
	return -1;
}

ostream& linvars::print(ostream & os) const {

	for (int i=0; i<n; ++i)
		os << vars[i] << endl;
	os << endl;

	return os;
}

const string* linvars::varnames() const {
	return vars;
}

ostream& operator<<(ostream & os, const linvars & a) {

	return a.print(os);
}


