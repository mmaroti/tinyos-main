#include "mapping.hpp"
using namespace std;

void mapping::add_var_array(const std::string arr[]) {

	const int n = sizeof(arr)/sizeof(string);

	for (int i=0; i<n; ++i)
		variables.push_back(arr[i]);
}

void mapping::add_variable(const string& var) {

	variables.push_back(var);
}

int mapping::size() {

	return static_cast<int> (variables.size());
}

