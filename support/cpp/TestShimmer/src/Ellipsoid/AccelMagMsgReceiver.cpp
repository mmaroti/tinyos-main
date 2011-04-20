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

#include <iostream>
#include "ActiveMessage.hpp"
#include "AccelMagMsgReceiver.hpp"
#include "MatrixVector.hpp"

using std::cout;
using std::endl;
using gyro::vector3;

const int ACCEL_SAMPLE_COUNT = 20;
const int TEMP_COUNT         = ACCEL_SAMPLE_COUNT;
const int MAG_SAMPLE_COUNT   = 4;

void AccelMagMsgReceiver::onReceiveMessage(const ActiveMessage& msg) {

    unsigned int t_mote = msg.getInt(0);

    vector3 accel(msg.getInt(4), msg.getInt(8), msg.getInt(12));

    accel /= ACCEL_SAMPLE_COUNT;

    const unsigned int acc_rng = msg.getShort(16);

    vector3 mag(msg.getInt(18), msg.getInt(22), msg.getInt(26));

    mag /= MAG_SAMPLE_COUNT;

    const unsigned int mag_rng = msg.getShort(30);

    unsigned int temp = msg.getInt(32);

    temp /= TEMP_COUNT;

    const unsigned int temp_rng = msg.getShort(36);

    cout << "Time: " << t_mote << endl;
    cout << "Accel: " << accel << ";  " << acc_rng << endl;
    cout << "Magn:  " << mag   << ";  " << mag_rng << endl;
    cout << "Temp:  " << temp << "; " << temp_rng << endl;
    cout << endl;
}
