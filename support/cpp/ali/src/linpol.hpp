#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <string>

class linpol {

public:

	linpol();

	linpol(const linpol& other);

	linpol(const std::string& name, double value);

	void set_coefficient(const std::string& name, double value);

	linpol& operator=(const linpol& rhs);

	friend const linpol operator+(const linpol& rhs, const linpol& lhs);

	~linpol();

	static void init_varnames(const std::string[]);

private:

	int find_index(const std::string& name);

	double* const val;

	static int size;

	static std::string* vars;
};

#endif

