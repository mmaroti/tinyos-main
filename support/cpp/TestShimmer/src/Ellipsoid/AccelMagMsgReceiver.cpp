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

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "ActiveMessage.hpp"
#include "AccelMagMsgReceiver.hpp"
#include "MatrixVector.hpp"

using namespace std;
using gyro::vector3;
using gyro::matrix3;

const int ACCEL_SAMPLE_COUNT = 100;
const int TEMP_COUNT         = ACCEL_SAMPLE_COUNT;
const int MAG_SAMPLE_COUNT   = 20;

class CalibrationMatrices {

public:

    CalibrationMatrices() { }

    CalibrationMatrices(const matrix3& A_acc, const vector3& b_acc,
                        const matrix3& A_magn, const vector3& b_magn)
    : A_acc(A_acc), b_acc(b_acc), A_magn(A_magn), b_magn(b_magn), AccScl(1,1,1), MagnScl(1,1,1)
    { }

    const vector3 accel(const vector3& x) const {

        using namespace gyro;

        vector3 y = A_acc*(x-b_acc)-AccOff;

        return vector3(AccScl[X]*y[X], AccScl[Y]*y[Y], AccScl[Z]*y[Z]);
    }

    const vector3 magn(const vector3 &x) const {

        using namespace gyro;

        vector3 y = A_magn*(x-b_magn)-MagnOff;

        return vector3(MagnScl[X]*y[X], MagnScl[Y]*y[Y], MagnScl[Z]*y[Z]);
    }

    void updateScaleOffset(const vector3& accScl,  const vector3& accOff,
                           const vector3& magnScl, const vector3& magnOff)
    {
        //AccScl = accScl;
        AccOff = accOff;
        //MagnScl = magnScl;
        MagnOff = magnOff;
    }

    void resetScaleOffset() {

        //AccScl = MagnScl = vector3(1,1,1);
        AccOff = MagnOff = vector3(0,0,0);
    }

private:

    matrix3 A_acc;
    vector3 b_acc;

    matrix3 A_magn;
    vector3 b_magn;

    vector3 AccScl;
    vector3 AccOff;

    vector3 MagnScl;
    vector3 MagnOff;
};

AccelMagMsgReceiver::AccelMagMsgReceiver()
    : calibrationMatrices(new map<int,CalibrationMatrices>)
{
    loadCalibrationMatrices();
}

void AccelMagMsgReceiver::loadCalibrationMatrices() {

    const QString CALIB_DIR("../calib");

    QDir dir(CALIB_DIR, "*.cal");

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {

        QString name = list.at(i).fileName();

        QFile file(CALIB_DIR+"/"+name);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QTextStream in(&file);

            loadFile(in);
        }
    }
}


void AccelMagMsgReceiver::loadFile(QTextStream& in) {

    if ("# Mote ID"!= in.readLine()) {

        return;
    }

    int moteID = in.readLine().toInt();

    QString buffer = in.readLine();

    if (buffer != "# Accelerometer A11, A12, A13, A22, A23, A33, B1, B2, B3; y = A(x-b)") {

        return;
    }

    matrix3 A_acc = loadMatrix(in);

    vector3 b_acc = loadVector(in);

    buffer = in.readLine();

    if (buffer != "# Magnetometer A11, A12, A13, A22, A23, A33, B1, B2, B3; y = A(x-b)") {

        return;
    }

    matrix3 A_magn = loadMatrix(in);

    vector3 b_magn = loadVector(in);

    QTextStream::Status status = in.status();

    if (status!=QTextStream::Ok) {

        return;
    }

    insertCalibrationMatrices(moteID, CalibrationMatrices(A_acc, b_acc, A_magn, b_magn));
}

const gyro::vector3 AccelMagMsgReceiver::loadVector(QTextStream& in) const {

    double x[3];

    for (int i=0; i<3; ++i) {

        x[i] = in.readLine().toDouble();
    }

    return vector3(x);
}

const gyro::matrix3 AccelMagMsgReceiver::loadMatrix(QTextStream& in) const {

    double x[6];

    for (int i=0; i<6; ++i) {

        x[i] = in.readLine().toDouble();
    }

    double values[] = { x[0], x[1], x[2],
                         0.0, x[3], x[4],
                         0.0,  0.0, x[5] };

    return matrix3(values);
}

void AccelMagMsgReceiver::insertCalibrationMatrices(int moteID, const CalibrationMatrices& M) {

    bool inserted = calibrationMatrices->insert(make_pair(moteID, M)).second;

    if (inserted) {

        cout << "Successfully loaded calibration data of mote " << moteID << endl;
    }
    else {

         cout << "Mote ID " << moteID << " already in use!" << endl;
    }
}

void AccelMagMsgReceiver::onReceiveMessage(const ActiveMessage& msg) {

    if (msg.type != 0x12) {

        return;
    }

    unsigned int t_mote = msg.getUInt(0);

    vector3 mag(msg.getSignedInt(4), msg.getSignedInt(8), msg.getSignedInt(12));

    mag /= MAG_SAMPLE_COUNT;

    vector3 accel(msg.getUInt(16), msg.getUInt(20), msg.getUInt(24));

    accel /= ACCEL_SAMPLE_COUNT;

    double temp = msg.getUInt(28);

    temp /= TEMP_COUNT;

    computeCalibratedVectors(msg.source, accel, mag);

    //cout << "Time: " << t_mote << endl;
    //cout << "Accel: " << accel << endl;
    //cout << "Magn:  " << mag   << endl;
    //cout << "Temp:  " << temp << endl;
    //cout << endl;

    emit newSample(AccelMagSample(msg.source, t_mote, accel, mag, temp));
}

bool AccelMagMsgReceiver::computeCalibratedVectors(int moteID, vector3& accel, vector3& magn) const {

    typedef map<int,CalibrationMatrices>::const_iterator itr;

    itr i = calibrationMatrices->find(moteID);

    if (i==calibrationMatrices->end()) {

        return false;
    }

    const CalibrationMatrices& M = i->second;

    accel = M.accel(accel);

    magn = M.magn(magn);

    return true;
}

typedef map<int,CalibrationMatrices>::iterator itr;

bool AccelMagMsgReceiver::updateScaleOffset(int moteID, const vector3& accScl,  const vector3& accOff,
                                                        const vector3& magnScl, const vector3& magnOff)
{
    itr i = calibrationMatrices->find(moteID);

    if (i==calibrationMatrices->end()) {

        return false;
    }

    CalibrationMatrices& M = i->second;

    M.updateScaleOffset(accScl, accOff, magnScl, magnOff);

    return true;
}

bool AccelMagMsgReceiver::resetScaleOffset(int moteID) {

    itr i = calibrationMatrices->find(moteID);

    if (i==calibrationMatrices->end()) {

        return false;
    }

    CalibrationMatrices& M = i->second;

    M.resetScaleOffset();

    return true;
}

AccelMagMsgReceiver::~AccelMagMsgReceiver() {

    delete calibrationMatrices;
}
