/* Copyright (c) 2011 University of Szeged
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

#ifndef ANGLERANGE_HPP
#define ANGLERANGE_HPP

#include <string>
#include "ArmTriplet.hpp"

class AngleRange {

public:

    AngleRange() { v[0] = v[1] = v[2] = v[3] = 0.0; }

    explicit AngleRange(double beg) { v[0] = v[1] = v[2] = v[3] = beg; }

    void next(double x);

    const std::string toPositiveLine(const char* lineStart) const { return positive().str(lineStart); }
    const std::string toNegativeLine(const char* lineStart) const { return negative().str(lineStart); }

    const std::string toPosCSV() const { return positive().toCSV(); }
    const std::string toNegCSV() const { return negative().toCSV(); }

    const std::string toCSV() const;

    double beg() const { return v[0]; }
    double end() const { return v[1]; }
    double min() const { return v[2]; }
    double max() const { return v[3]; }

    double range() const { return max()-min(); }

private:

    const AngleRange positive() const;
    const AngleRange negative() const;

    const std::string str(const char* name) const;

    void min(double x) { v[2] = x; }
    void max(double x) { v[3] = x; }
    void end(double x) { v[1] = x; }

    double v[4];
};

#endif // ANGLERANGE_HPP
