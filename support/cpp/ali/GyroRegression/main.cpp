#include <iostream>
#include "Optimizer.hpp"
#include "InputData.hpp"
#include "DataImporter.hpp"

using namespace std;

int main() {

	input data;

	// FIXME Memory leak
	read_file("manual", data);

	Optimizer opt(data, cout);

	return 0;
}

