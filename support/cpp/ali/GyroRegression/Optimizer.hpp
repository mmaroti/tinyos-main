#ifndef OPTIMIZER_HPP_
#define OPTIMIZER_HPP_

#include <iosfwd>
#include "InputData.hpp"

class Optimizer {

public:

	Optimizer(const input& data, std::ostream& os) :
			data(data), os(os)
	{

	}

	Optimizer(const char* filename, std::ostream& os);

	// Returns true if OK
	bool run();

private:

	input data;
	std::ostream& os;
};

#endif
