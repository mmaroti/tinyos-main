#include <iostream>
#include "Optimizer.hpp"
#include "InputData.hpp"
#include "DataImporter.hpp"

using namespace std;

int main() {

	input data = read_file("manual");

	Optimizer opt(data, cout);

	data.release_resources();

	return 0;
}

