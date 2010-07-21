#include <iostream>
#include <memory>
#include "Optimizer.hpp"
#include "DataImporter.hpp"

using namespace std;

using namespace gyro;

int main() {

	auto_ptr<const input> data(read_file("manual"));

	Optimizer opt(*data, cout);

	data.reset();

	const double* const x = opt.solution();

	for (int i=0; i<opt.n_vars(); ++i)
		cout << "x[" << i << "]\t" << x[i] << endl;

	cout << endl;

	cout << "Error in g (in m/s^2) : " << opt.error_in_g() << endl;

	return 0;
}

