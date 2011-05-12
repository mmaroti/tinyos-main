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

// TODO upper arm matrix in flex and dev

const ArmAngles ArmAngles::left() {

    return ArmAngles(ElbowFlexSign::left());
}

const ArmAngles ArmAngles::right() {

    return ArmAngles(ElbowFlexSign::right());
}

ArmAngles::ArmAngles(ElbowFlexSign s) : sign(s) {

    heading[FOREARM] = heading[UPPERARM] = 0.0;
}

void ArmAngles::dumpAngles(const map<int,matrix3>& matrices) const {

    if (matrices.empty()) {

        return;
    }

    typedef map<int,matrix3>::const_iterator itr;

    itr i = matrices.begin();

    cout << "Forearm" << endl;

    vector<matrix3> rotmat;

    rotmat.push_back( heading2rotation(heading[FOREARM])*(i->second) );

    //angles2stdout( rotmat.at(0) );

    ++i;

    if (i!=matrices.end()) {

        cout << "Upper arm" << endl;

        rotmat.push_back( heading2rotation(heading[UPPERARM])*(i->second) );
    }
    else {

        rotmat.push_back( matrix3(0, -1, 0, 1, 0, 0, 0, 0, 1) );
    }

    angles2stdout( rotmat.at(1) );
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
    // FIXME Tilt compensation?
    cout << "x: " << rotMat[X] << endl;
    cout << "y: " << rotMat[Y] << endl;
    cout << "z: " << rotMat[Z] << endl;

    double heading = atan2(rotMat[Y][Y], rotMat[Y][X])*RAD2DEG;

    cout << heading << endl << endl;

    return heading;
}

const matrix3 ArmAngles::heading2rotation(double heading) const {

    const double headingRAD = heading / RAD2DEG;

    const double s = sin(headingRAD); // All trigonometric computations could have been
    const double c = cos(headingRAD); // avoided here and in magneticHeading()

    return matrix3( 1.0, 0.0, 0.0,
                    0.0,   c,  -s,
                    0.0,   s,   c );
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

double ArmAngles::armFlex(const std::vector<gyro::matrix3>& rotmat) const {

    double upperFlex = flexion(rotmat.at(UPPERARM));

    double foreFlex  = flexion(rotmat.at(FOREARM));

    double flex = foreFlex - upperFlex;

    if (flex > 180) {

        flex -= 360;
    }
    else if (flex < -180) {

        flex += 360;
    }

    return flex;
}

double ArmAngles::armDev(const std::vector<gyro::matrix3>& rotmat) const {

    double foreDev  = deviation(rotmat.at(FOREARM));

    double upperDev = deviation(rotmat.at(UPPERARM));

    return (fabs(foreDev) > fabs(upperDev) ) ? foreDev : upperDev;
}

const ArmAngles::Triplet ArmAngles::getAngles(const std::map<int,gyro::matrix3>& matrices) const {

    vector<matrix3> rotmat = fillUp(matrices);

    Triplet t;

    t.flex   = armFlex(rotmat);

    t.sup    = supination(rotmat.at(FOREARM));

    t.dev = armDev(rotmat);

    return t;
}

const vector<matrix3> ArmAngles::fillUp(const map<int,matrix3>& mat) const {

    typedef map<int,matrix3>::const_iterator itr;

    itr i   = mat.begin();

    itr end = mat.end();

    vector<matrix3> matrices;

    for (int k=0; k<2; ++k) {

        matrix3 m;

        if (i!=end) {
            m = heading2rotation(heading[k])*(i->second);
            ++i;
        }

        else {
            m = matrix3(0, -1, 0, 1, 0, 0, 0, 0, 1);
        }

        matrices.push_back(m);
    }

    return matrices;
}

const std::vector<std::string> ArmAngles::labels(const std::map<int,gyro::matrix3>& matrices) const {

    const Triplet t = getAngles(matrices);

    vector<string> label;

    label.push_back(angle2str(t.flex, "Flex", "Ext"));

    label.push_back(angle2str(t.sup, "Sup","Pron"));

    label.push_back(angle2str(t.dev, "Lat dev", "Med dev"));

    return label;
}

const vector<string> ArmAngles::table(vector<map<int,matrix3> >& frames) const {

    vector<string> table(6);

    if (frames.empty()) {

        return table;
    }

    Triplet t = getAngles(frames.at(0));

    AngleRange flex(t.flex), sup(t.sup), dev(t.dev);

    for (size_t i=1; i<frames.size(); ++i) {

        t = getAngles(frames.at(i));

        flex.next(t.flex);
        sup.next(t.sup);
        dev.next(t.dev);
    }

    return fillTable(table, flex, sup, dev);
}

vector<std::string>& ArmAngles::fillTable(vector<string>& table,
                               const AngleRange& flex,
                               const AngleRange& sup,
                               const AngleRange& dev) const
{
    table.at(0) = flex.positive().str("Flex ");
    table.at(1) = flex.negative().str("Ext ");
    table.at(2) = sup.positive().str("Sup ");
    table.at(3) = sup.negative().str("Pron ");
    table.at(4) = dev.positive().str("Lat dev ");
    table.at(5) = dev.negative().str("Med dev ");

    return table;
}
