#ifndef MAPPING_HPP_
#define MAPPING_HPP_

#include <string>
#include <list>

class mapping {

public:

	void add_variable(const std::string& var);

	void add_var_array(const std::string arr[]);

	int size();

private:

	std::list<std::string> variables;

};

#endif /* MAPPING_HPP_ */
