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

#ifndef DATAPLOT_H
#define DATAPLOT_H
#define RADIAN 57.2957795

#include <QWidget>
#include <QPen>
#include "cmath"
#include "CalibrationModule.h"
#include "Application.h"


class PlotScrollArea;
class Application;
class DataRecorder;
class QPen;
class QTimerEvent;

class DataPlot : public QWidget
{
        Q_OBJECT
public:
        DataPlot(PlotScrollArea *parent, Application &app);

        enum
        {
                XRAWACC   = 0x0001,
                YRAWACC   = 0x0002,
                ZRAWACC   = 0x0004,
                XRAWGYRO  = 0x0008,
                YRAWGYRO  = 0x0010,
                ZRAWGYRO  = 0x0020,
                XACC      = 0x0040,
                YACC      = 0x0080,
                ZACC      = 0x0100,
                XGYRO     = 0x0200,
                YGYRO     = 0x0400,
                ZGYRO     = 0x0800,
                ABSACC    = 0x1000,
                XYANG     = 0x2000,
                YZANG     = 0x4000,
                ZXANG     = 0x8000,

                VOLTAGE = 0x10000,
                GRID = 0x20000,
                TIME = 0x40000,
                TEMP = 0x80000,
        };

        void setGraphs(int graphs, bool on);
        int getGraphs() const { return graphs; }

        double calibrationDataAt(int i) { return calibrationData[i]; }
        double gyroMinAvgsAt(int i) { return gyroMinAvgs[i]; }
        double gyroCalibrationDataAt(int i) { return gyroCalibrationData[i]; }
        double calculateAngle( double acceleration1, double acceleration2 );
        double calculateCalibratedValue( QString axis, int time );
        void loadSettingsData();
        void onNewCalibration();

protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent * event);

        int graphs;

public slots:
        void onSampleAdded();
        void onSamplesCleared();

signals:
        void angleChanged(double angle);

private:
        Application &application;
        PlotScrollArea *scrollArea;
        int parentHeight;
        int plotWidth;

        QPoint getPoint(int x, int y);
        QPoint getSample(int x, int y);

        double gyroMinAvgs[3];
        double gyroCalibrationData[12];
        double calibrationData[12];
};

#endif // DATAPLOT_H
