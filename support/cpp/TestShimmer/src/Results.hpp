/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Ali Baharev
*/

#ifndef RESULTS_HPP
#define RESULTS_HPP

#include "CompileTimeConstants.hpp"

// Just a chunk of data at the moment...

namespace ipo {

class Results {

public:

    friend class DataReader;

    Results() : n(0), m(0) { }

    ~Results() { delete[] m; n = 0; m = 0; }

    const double* var() const { return x; }

    const double* var_lb() const { return x_lb; }

    const double* var_ub() const { return x_ub; }

    int number_of_samples() const { return n; }

    const double* rotation_matrices() const { return m; }

    double R(int sample, int i, int j) const;

private:

    Results(const Results& );

    Results& operator=(const Results& );

    double x[gyro::NUMBER_OF_VARIABLES];

    double x_lb[gyro::NUMBER_OF_VARIABLES];

    double x_ub[gyro::NUMBER_OF_VARIABLES];

    int n;

    double* m;

};

}

#endif
