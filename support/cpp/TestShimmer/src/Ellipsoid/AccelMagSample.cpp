/* Copyright (c) 2011, University of Szeged
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
*      Author: Ali Baharev
*/

#include <iomanip>
#include <ostream>
#include <sstream>
#include "AccelMagSample.hpp"

using namespace gyro;

AccelMagSample::AccelMagSample()
: mote_id(-1), time(0), accel(vector3(0,0,0)), mag(vector3(0,0,0)), temp(0)
{

}

AccelMagSample::AccelMagSample(int mote_id,
                               unsigned int mote_time,
                               const vector3& acceleration,
                               const vector3& magnetometer_reading,
                               double temperature)
: mote_id(mote_id), time(mote_time), accel(acceleration), mag(magnetometer_reading), temp(temperature)
{

}

const QString AccelMagSample::timeStr() const {

    return QString::number(time);
}

const QString vec2QStr(const gyro::vector3& v) {

    using namespace std;

    ostringstream os;

    os << fixed << setprecision(2) << right;

    os << setw(6) << v[X] << ", " << setw(6) << v[Y] << ", " << setw(6) << v[Z];

    return os.str().c_str();
}

const QString AccelMagSample::accelStr() const {

    return vec2QStr(accel);
}

const QString AccelMagSample::magStr() const {

    return vec2QStr(mag);
}

const QString AccelMagSample::tempStr() const {

    return QString::number(temp);
}

