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
#include "AngleRange.hpp"

using gyro::matrix3;
using std::string;
using std::vector;

typedef std::map<int,matrix3> Frame;      // Either forearm only or whole arm
typedef vector<matrix3>       WholeFrame; // Whole arm
typedef vector<Frame>         FrameVec;

typedef ArmTriplet<double>     AngleTriplet;
typedef ArmTriplet<AngleRange> RangeTriplet;


class ArmAngles {

public:

    static const ArmAngles left();

    static const ArmAngles right();

    const vector<double> setHeading(const Frame& matrices);

    const vector<double> setHeading(const vector<double>& headings);

    const vector<string> labels(const Frame& matrices, size_t frameIndex, size_t size) const;

    const vector<string> table(const FrameVec& frames) const;

    const string anglesCSV(const Frame& matrices) const;

    const string tableCSV(const FrameVec& frames) const;

    const string type() const;

private:

    ArmAngles(ElbowFlexSign s);

    const AngleTriplet getAngles(const Frame& matrices) const;

    const WholeFrame headingCorrectedWholeFrame(const Frame& matrices) const;

    double magneticHeading(const matrix3& m) const;

    const matrix3 heading2rotation(double heading) const;

    double armFlex(const WholeFrame& matrices) const;

    double flexion(const matrix3& m) const;

    double supination(const matrix3& m) const;

    double armDev(const WholeFrame& matrices) const;

    double deviation(const matrix3& m) const;

    const string angle2str(double angle, const char* positive, const char* negative) const;

    const string frameLabel(size_t frameIndex, size_t size) const;

    const RangeTriplet getRanges(const FrameVec& frames) const;

    vector<string>& fillTable(vector<string>& table, const RangeTriplet& ranges) const;

    const ElbowFlexSign sign;

    enum { FOREARM, UPPERARM };

    double heading[2];

};

#endif // ARMANGLES_HPP
