/** Copyright (c) 2010, University of Szeged
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
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#ifndef PERIODICALCALIBRATIONMODULE_H
#define PERIODICALCALIBRATIONMODULE_H
#define MAXDIFF 20
#define NUMOFROT 10.0

#include <QObject>
#include "Application.h"

class Application;

struct IdleGyroWindow {
    IdleGyroWindow();
    QString toString() const;

    int size;
    float numOfRotations;
    double alfaSum;
    long Xint, Yint, Zint;
};

class PeriodicalCalibrationModule : public QObject {
    Q_OBJECT
public:
    PeriodicalCalibrationModule( Application &app, CalibrationModule &calMod );
    ~PeriodicalCalibrationModule();

    QString Calibrate(QString rotAxis);
    QString SVD();

    double getCalibratedData(int time, QString axis);
    double calculateAngle(double accel1, double accel2);

    QString printSettingsToConsole(QString settingsData);
    void ClearWindows();
private:
    Application &application;
    CalibrationModule &calibrationModule;

    double gyroMinAvgs[3];
    QVector<IdleGyroWindow> idleGyroWindows;
    double gyroCalibrationData[12];
};

#endif // PERIODICALCALIBRATIONMODULE_H
