#ifndef LINPOL_HPP_
#define LINPOL_HPP_

#include <iosfwd>
#include <string>

class linvars;

class linpol {

public:

	linpol() : val(0), names(0) { }

	explicit linpol(const linvars* const name);

	linpol(const linpol& other);

	linpol& set_constant(double value);

	linpol& set_coefficient(const std::string& name, double value);

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

private:

	void init_if_needed(const linpol& other);

	double* val;

	const linvars* names;
};

std::ostream& operator<<(std::ostream& os, const linpol& a);

#endif
