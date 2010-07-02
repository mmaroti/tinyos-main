#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <string>

class linpol {

public:

	linpol();

	linpol(const linpol& other);

	linpol& operator=(const linpol& rhs);

	void set_coefficient(const std::string& name, double value);

	~linpol();

	static void init_varnames(const std::string[]);

private:

	int find_index(const std::string& name);

	double* const val;

	static int size;

	static std::string* vars;
};

#endif

