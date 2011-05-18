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

#ifndef ACCELMAGSAMPLE_HPP
#define ACCELMAGSAMPLE_HPP

#include <QString>
#include "MatrixVector.hpp"

const QString vec2QStr(const gyro::vector3& v);

class AccelMagSample {

public:

    AccelMagSample();

    AccelMagSample(int mote_id,
                   unsigned int mote_time,
                   const gyro::vector3& acceleration,
                   const gyro::vector3& magnetometer_reading,
                   double temperature);

    bool isNull() const { return mote_id==-1 && time==0; }

    int moteID() const { return mote_id; }

    unsigned int moteTime() const { return time; }

    const gyro::vector3 acceleration() const { return accel; }

    const gyro::vector3 magnetometerReading() const { return mag; }

    double temperature() const { return temp; }

    bool isStatic() const;

    const gyro::matrix3 toRotationMatrix() const;

    double acceleration(gyro::coordinate c) const { return accel[c]; }

    const QString timeStr() const;

    const QString accelStr() const;

    const QString magStr() const;

    const QString tempStr() const;

private:

    int mote_id;
    unsigned int time;
    gyro::vector3 accel;
    gyro::vector3 mag;
    double temp;
};

#endif // ACCELMAGSAMPLE_HPP
