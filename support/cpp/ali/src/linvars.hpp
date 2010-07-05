#ifndef LINVARS_HPP_
#define LINVARS_HPP_

#include <iosfwd>
#include <string>

class linvars {

public:

	linvars(const std::string arr[], int length);

	int size() const;

	int find_index(const std::string& name) const;

	std::ostream& print(std::ostream& os) const;

	const std::string* varnames() const;

	~linvars();

private:

	linvars(const linvars& );

	linvars& operator=(const linvars& );

	int n;

	std::string* vars;
};

std::ostream& operator<<(std::ostream& os, const linvars& a);

#endif
