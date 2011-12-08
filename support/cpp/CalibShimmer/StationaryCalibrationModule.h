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

#ifndef STATIONARYCALIBRATIONMODULE_H
#define STATIONARYCALIBRATIONMODULE_H

#include <QObject>
#include <jama_qr.h>
#include <QVarLengthArray>
#include <QStringList>
#include <QVector>
#include "constants.h"

class Application;
class MoteData;

/*!
  A structure to store the averages in the actual WINDOW length data-window.
  We only create this structure, if we consider the mote to be idle by the data in this window.
*/
struct IdleWindow
{
        IdleWindow();
        QString toString() const;

        int start;
        int xMin, xMax;
        double xAvg;
        int yMin, yMax;
        double yAvg;
        int zMin, zMax;
        double zAvg;
        double xGyroAvg, yGyroAvg, zGyroAvg;
};

class StationaryCalibrationModule : public QObject {
    Q_OBJECT
public:
    StationaryCalibrationModule( Application &app );
    ~StationaryCalibrationModule();

    QString Calibrate(MoteData*);
    QString Classify();
    QString LSF();
    void saveCalibratedData(QString);
    void printMatrix1D(TNT::Array1D<double>);
    void printMatrix2D(TNT::Array2D<double>);

    const QVarLengthArray<IdleWindow> & getIdleWindows() const {
            return idleWindows;
    }

    int size() const { return idleWindows.size(); }

    const IdleWindow & at(int i) const { return idleWindows[i]; }

    QString atToString(int i) const { return idleWindows[i].toString(); }

    void clearWindows();

    int atIdleSides(int i) const { return idleSidesMins[i]; }

    void clearIdleSides();

    int idleSidesMins[6];
    double gyroMinAvgs[3];
    QVector<double> solutions;

private:
    Application &application;

    int xMin, xMax, yMin, yMax, zMin, zMax, xDiff, yDiff, zDiff;
    int xGyrMin, xGyrMax, xGyrDiff, yGyrMin, yGyrMax, yGyrDiff, zGyrMin, zGyrMax, zGyrDiff;
    float xAvg, yAvg, zAvg;
    float xMinAvg, xMaxAvg, yMinAvg, yMaxAvg, zMinAvg, zMaxAvg;
    float xGyrAvg, yGyrAvg, zGyrAvg;
    float xMinGyrAvg, xMaxGyrAvg, yMinGyrAvg, yMaxGyrAvg, zMinGyrAvg, zMaxGyrAvg;
    long xSum, ySum, zSum;
    long xGyrSum, yGyrSum, zGyrSum;

    QStringList variableNames;
    //QMessageBox msgBox;

    QVarLengthArray<IdleWindow> idleWindows;


};

#endif // StationaryCalibrationModule_H
