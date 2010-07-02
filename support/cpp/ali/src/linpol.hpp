#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <iosfwd>
#include <string>

class linpol {

public:

	linpol();

	linpol(const linpol& other);

	linpol(const std::string& name, double value);

	void set_constant(double value);

	void set_coefficient(const std::string& name, double value);

	linpol& operator=(const linpol& rhs);

	friend const linpol operator-(const linpol& pol);

	linpol& operator+=(const linpol& rhs);

	linpol& operator-=(const linpol& rhs);

	friend const linpol operator*(const linpol& lhs, const linpol& rhs);

	~linpol();

	std::ostream& print(std::ostream& os) const;

	static void init_varnames(const std::string[], int length);

	static void print_varnames();

private:

	int find_index(const std::string& name);

	double* const val;

	static int size;

	static std::string* vars;
};

const linpol operator+(const linpol& lhs, const linpol& rhs);

const linpol operator-(const linpol& lhs, const linpol& rhs);

std::ostream& operator<<(std::ostream& os, const linpol& a);

#endif
