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

#ifndef TURNTABLECALIBRATIONMODULE_H
#define TURNTABLECALIBRATIONMODULE_H

/*#define GYROWINDOW 200  //data window size, for finding idle shimmer positions
#define GYROMAXDIFF 20  //maximum difference between max and min values from an idle mote
#define xCCLW   0.9*xMinGyroAvg+0.1*xMaxGyroAvg  //region borders for classifying idle windows
#define xI_L    0.7*xMinGyroAvg+0.3*xMaxGyroAvg  //CLW = ClockWise rotation
#define xI_U    0.3*xMinGyroAvg+0.7*xMaxGyroAvg  //CCLW = counter ClockWise rotation
#define xCLW    0.1*xMinGyroAvg+0.9*xMaxGyroAvg  //I = Idle
#define yCCLW   0.9*yMinGyroAvg+0.1*yMaxGyroAvg  //L = lower border; U = upper border;
#define yI_L    0.7*yMinGyroAvg+0.3*yMaxGyroAvg  //x,y,z : axis
#define yI_U    0.3*yMinGyroAvg+0.7*yMaxGyroAvg
#define yCLW    0.1*yMinGyroAvg+0.9*yMaxGyroAvg
#define zCCLW   0.9*zMinGyroAvg+0.1*zMaxGyroAvg
#define zI_L    0.7*zMinGyroAvg+0.3*zMaxGyroAvg
#define zI_U    0.3*zMinGyroAvg+0.7*zMaxGyroAvg
#define zCLW    0.1*zMinGyroAvg+0.9*zMaxGyroAvg*/

#include "constants.h"

#include <QObject>
#include <QVarLengthArray>
#include <jama_qr.h>

class Application;
class MoteData;

/*!
  A structure to store the averages in the actual WINDOW length data-window.
  We only create this structure, if we consider the mote to be idle by the data in this window.
*/
struct TurntableWindow
{
        TurntableWindow();
        QString toString() const;

        long time;
        int start;

        int xGyroMin, xGyroMax;
        int yGyroMin, yGyroMax;
        int zGyroMin, zGyroMax;

        double xGyroAvg, yGyroAvg, zGyroAvg;
};

class TurntableCalibrationModule : public QObject {
    Q_OBJECT
public:
    TurntableCalibrationModule( Application &app );
    ~TurntableCalibrationModule();

    QString Calibrate(MoteData*, int rpm);
    QString Classify();
    QString LSF();
    void saveCalibratedData(QString);
    void printMatrix1D(TNT::Array1D<double>);
    void printMatrix2D(TNT::Array2D<double>);

    const QVarLengthArray<TurntableWindow> & getTurntableWindows() const {
            return turntableWindows;
    }

    int size() const { return turntableWindows.size(); }

    const TurntableWindow & at(int i) const { return turntableWindows[i]; }

    QString atToString(int i) const { return turntableWindows[i].toString(); }

    void clearWindows();

    const int & atTurntableSides(int i) const { return turntableSidesMins[i]; }

    void clearTurntableSides();

private:
    Application &application;

    int xGyroMin, xGyroMax, xGyroDiff, yGyroMin, yGyroMax, yGyroDiff, zGyroMin, zGyroMax, zGyroDiff;
    double xGyroAvg, yGyroAvg, zGyroAvg;
    double xMinGyroAvg, xMaxGyroAvg, yMinGyroAvg, yMaxGyroAvg, zMinGyroAvg, zMaxGyroAvg;
    long xGyroSum, yGyroSum, zGyroSum;
    double constant;

    QVarLengthArray<TurntableWindow> turntableWindows;
    int turntableSidesMins[7];

};

#endif // TURNTABLECALIBRATIONMODULE_H
