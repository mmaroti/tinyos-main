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

#ifndef ARMANGLES_HPP
#define ARMANGLES_HPP

#include <map>
#include <string>
#include <vector>
#include "ElbowFlexSign.hpp"
#include "MatrixVector.hpp"

class ArmAngles {

public:

    static const ArmAngles left();
    static const ArmAngles right();

    const std::vector<double> setHeading(const std::map<int,gyro::matrix3>& matrices);

    void dumpAngles(const std::map<int,gyro::matrix3>& matrices) const;

private:

    ArmAngles(ElbowFlexSign s);

    void angles2stdout(const gyro::matrix3& m) const;

    double flexion(const gyro::matrix3& m) const;
    double supination(const gyro::matrix3& m) const;
    double deviation(const gyro::matrix3& m) const;

    double magneticHeading(const gyro::matrix3& m) const;

    const gyro::matrix3 heading2rotation(double heading) const;

    const std::string flexStr(const gyro::matrix3& m) const;
    const std::string supStr(const gyro::matrix3& m) const;
    const std::string devStr(const gyro::matrix3& m) const;

    std::string angle2str(double angle, const char* positive, const char* negative) const;

    const ElbowFlexSign sign;

    enum { FOREARM, UPPERARM };

    double heading[2];

};

#endif // ARMANGLES_HPP
