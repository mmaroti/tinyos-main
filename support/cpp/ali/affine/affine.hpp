#ifndef AFFINE_HPP_
#define AFFINE_HPP_

#include <iosfwd>

enum {
	CONST,
	X,
	Y,
	SIZE
};

class affine {

public:

	affine();

	affine(const affine& other);

	affine& operator=(const affine& rhs);

	friend const affine operator+(const affine& lhs, const affine& rhs);

	friend const affine operator-(const affine& lhs, const affine& rhs);

	double  operator[](int i) const;

	double& operator[](int i);

	std::ostream& print(std::ostream& os) const;

private:

	double val[SIZE+1];
};

std::ostream& operator<<(std::ostream& os, const affine& a);

#endif
