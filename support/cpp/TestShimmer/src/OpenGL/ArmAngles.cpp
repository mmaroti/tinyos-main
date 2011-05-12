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
#include <sstream>
#include "ArmAngles.hpp"

using namespace std;
using namespace gyro;

namespace {

    const double RAD2DEG = 57.2957795131;
    const double PI_HALF = 1.57079632679;
}

const ArmAngles ArmAngles::left() {

    return ArmAngles(ElbowFlexSign::left());
}

const ArmAngles ArmAngles::right() {

    return ArmAngles(ElbowFlexSign::right());
}

ArmAngles::ArmAngles(ElbowFlexSign s) : sign(s) {

    heading[FOREARM] = heading[UPPERARM] = 0.0;
}

const vector<double> ArmAngles::setHeading(const Frame& matrices) {

    typedef Frame::const_iterator itr;

    int k=0;

    for (itr i=matrices.begin(); i!=matrices.end() && k<2; ++i, ++k) {

        const matrix3& rotMat = i->second;

        heading[k] = magneticHeading(rotMat) + 90.0;
    }

    return vector<double>(heading, heading+2);
}

double ArmAngles::magneticHeading(const matrix3& rotMat) const {
    // TODO Tilt compensation?
    //cout << "x: " << rotMat[X] << endl;
    //cout << "y: " << rotMat[Y] << endl;
    //cout << "z: " << rotMat[Z] << endl;

    double heading = atan2(rotMat[Y][Y], rotMat[Y][X])*RAD2DEG;

    //cout << heading << endl << endl;

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

const vector<string> ArmAngles::labels(const Frame& matrices, size_t index, size_t size) const {

    const AngleTriplet t = getAngles(matrices);

    vector<string> label;

    label.push_back(angle2str(t.flex, "Flex", "Ext"));

    label.push_back(angle2str(t.sup, "Sup","Pron"));

    label.push_back(angle2str(t.dev, "Lat dev", "Med dev"));

    label.push_back(frameLabel(index, size));

    return label;
}

const AngleTriplet ArmAngles::getAngles(const Frame& mat) const {

    WholeFrame matrices = headingCorrectedWholeFrame(mat);

    AngleTriplet t;

    t.flex   = armFlex(matrices);

    t.sup    = supination(matrices.at(FOREARM));

    t.dev    = armDev(matrices);

    return t;
}

const WholeFrame ArmAngles::headingCorrectedWholeFrame(const Frame& mat) const {

    typedef Frame::const_iterator itr;

    itr i   = mat.begin();

    itr end = mat.end();

    WholeFrame matrices;

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

double ArmAngles::armFlex(const WholeFrame& matrices) const {

    double upperFlex = flexion(matrices.at(UPPERARM));

    double foreFlex  = flexion(matrices.at(FOREARM));

    double flex = foreFlex - upperFlex;

    if (flex > 180) {

        flex -= 360;
    }
    else if (flex < -180) {

        flex += 360;
    }

    return flex;
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

double ArmAngles::armDev(const WholeFrame& matrices) const {

    double foreDev  = deviation(matrices.at(FOREARM));

    double upperDev = deviation(matrices.at(UPPERARM));

    return (fabs(foreDev) > fabs(upperDev) ) ? foreDev : upperDev;
}

double ArmAngles::deviation(const matrix3& m) const {

    return (acos(m[Z][Y])-PI_HALF)*RAD2DEG*(sign.dev);
}

const string ArmAngles::angle2str(double angle, const char* positive, const char* negative) const {

    ostringstream os;

    os << setprecision(1) << fixed;

    if (angle >= 0.0) {

        os << positive << ": ";
    }
    else {

        os << negative << ": ";
    }

    os << abs(angle) << " deg";

    return os.str();
}

const string ArmAngles::frameLabel(size_t frameIndex, size_t size) const {

    ostringstream os;

    os << "Frame: " << ((size==0)?0:frameIndex+1) << " of " << size;

    return os.str();
}

const vector<string> ArmAngles::table(const FrameVec& frames) const {

    vector<string> table(6); // TODO Knows size... program crash if not updated properly

    if (frames.empty()) {

        return table;
    }

    AngleTriplet t = getAngles(frames.at(0));

    AngleRange flex(t.flex), sup(t.sup), dev(t.dev);

    for (size_t i=1; i<frames.size(); ++i) {

        t = getAngles(frames.at(i));

        flex.next(t.flex);
         sup.next(t.sup);
         dev.next(t.dev);
    }

    return fillTable(table, RangeTriplet(flex, sup, dev));
}

vector<string>& ArmAngles::fillTable(vector<string>& table, const RangeTriplet& t) const
{

    table.at(0) = t.flex.toPositiveLine("Flex ");
    table.at(1) = t.flex.toNegativeLine("Ext ");
    table.at(2) = t.sup.toPositiveLine("Sup ");
    table.at(3) = t.sup.toNegativeLine("Pron ");
    table.at(4) = t.dev.toPositiveLine("Lat dev ");
    table.at(5) = t.dev.toNegativeLine("Med dev ");

    return table;
}
