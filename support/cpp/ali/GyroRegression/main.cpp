#include <iostream>
#include "GyroNLP.hpp"
#include "InputData.hpp"

int main() {

	input data;
	data.acc_x = 0;
	data.acc_y = 0;
	data.acc_z = 0;
	data.wx = 0;
	data.wy = 0;
	data.wz = 0;
	data.N = 0;
	data.dt = 0;
	data.g_ref = 0;

	GyroNLP gnlp(data,std::cout);

	return 0;
}

