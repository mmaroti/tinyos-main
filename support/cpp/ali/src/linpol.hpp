#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <iosfwd>
#include <string>

class linpol {

public:

	// Initializes all elements to zero
	linpol();

	linpol(const linpol& other);

	linpol(const std::string& name, double value);

	void set_constant(double value);

	void set_coefficient(const std::string& name, double value);

	linpol& operator=(const linpol& rhs);

	// Out of necessity, only works with rhs = 0
	linpol& operator=(double rhs);

	linpol& operator+=(const linpol& rhs);

	linpol& operator-=(const linpol& rhs);

	friend const linpol operator-(const linpol& pol);

	friend const linpol operator+(const linpol& lhs, const linpol& rhs);

	friend const linpol operator-(const linpol& lhs, const linpol& rhs);

	friend const linpol operator*(const linpol& lhs, const linpol& rhs);

	~linpol();

	std::ostream& print(std::ostream& os) const;

	static void init_varnames(const std::string[], int length);

	static void print_varnames();

private:

	// Leaves the elements uninitialized -- messing with the fire
	explicit linpol(bool dummy);

	int find_index(const std::string& name);

	double* const val;

	static int size;

	static std::string* vars;
};

std::ostream& operator<<(std::ostream& os, const linpol& a);

#endif
