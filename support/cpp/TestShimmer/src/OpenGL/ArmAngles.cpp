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

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "ArmAngles.hpp"

using namespace std;
using namespace gyro;

namespace {

    const double RAD2DEG = 57.2957795131;
    const double PI_HALF = 1.57079632679;
}

// TODO Forearm upper arm ref heading; upper arm matrix in flex and dev

const ArmAngles ArmAngles::left() {

    return ArmAngles(ElbowFlexSign::left());
}

const ArmAngles ArmAngles::right() {

    return ArmAngles(ElbowFlexSign::right());
}

ArmAngles::ArmAngles(ElbowFlexSign s) : sign(s) {

    heading[FOREARM] = heading[UPPERARM] = 0.0;
}

// TODO Rotate to reference here? Move rotation from the ArmWidget to here?
void ArmAngles::dumpAngles(const map<int,matrix3>& matrices) const {

    if (matrices.empty()) {

        return;
    }

    typedef map<int,matrix3>::const_iterator itr;

    itr i = matrices.begin();

    cout << "Forearm" << endl;

    angles2stdout(i->second);

    ++i;

    if (i==matrices.end()) {

        return;
    }

    cout << "Upper arm" << endl;

    angles2stdout(i->second);
}

const vector<double> ArmAngles::setHeading(const map<int,matrix3>& matrices) {

    typedef map<int,matrix3>::const_iterator itr;

    int k=0;

    for (itr i=matrices.begin(); i!=matrices.end() && k<2; ++i, ++k) {

        heading[k] = magneticHeading(i->second) + 90.0;
    }

    return vector<double>(heading, heading+2);
}

double ArmAngles::flexion(const matrix3& m) const {

    double flex = atan2(m[X][Y], -m[Y][Y])*RAD2DEG + 90.0;

    if (flex > 180.0) {

        flex -= 360.0;
    }

    return flex;
}

double ArmAngles::supination(const matrix3& m) const {

    return -atan2(m[Z][X],m[Z][Z])*RAD2DEG*(sign.sup);
}

double ArmAngles::deviation(const matrix3& m) const {

    return (acos(m[Z][Y])-PI_HALF)*RAD2DEG*(sign.dev);
}

std::string ArmAngles::angle2str(double angle, const char* positive, const char* negative) const {

    ostringstream os;

    os << setprecision(1) << fixed;

    if (angle >= 0.0) {

        os << positive << ": ";
    }
    else {

        os << negative << ": ";
    }

    os << std::abs(angle) << " deg";

    return os.str();
}

const std::string ArmAngles::flexStr(const matrix3& m) const {

    return angle2str(flexion(m), "Flex", "Ext");
}

const std::string ArmAngles::supStr(const matrix3& m) const {

    return angle2str(supination(m), "Sup", "Pron");
}

const std::string ArmAngles::devStr(const matrix3& m) const {

    return angle2str(deviation(m), "Lat dev", "Med dev");
}

double ArmAngles::magneticHeading(const gyro::matrix3& rotMat) const {

    cout << "x: " << rotMat[X] << endl;
    cout << "y: " << rotMat[Y] << endl;
    cout << "z: " << rotMat[Z] << endl;

    double heading = atan2(rotMat[Y][Y], rotMat[Y][X])*RAD2DEG;

    cout << heading << endl << endl;

    return heading;
}

void ArmAngles::angles2stdout(const gyro::matrix3& m) const {

    cout << setprecision(2) << fixed;

    cout << "x: " << m[X] << endl;
    cout << "y: " << m[Y] << endl;
    cout << "z: " << m[Z] << endl;

    cout << flexStr(m) << endl;
    cout << supStr(m) << endl;
    cout << devStr(m) << endl;
    cout << endl;
}
