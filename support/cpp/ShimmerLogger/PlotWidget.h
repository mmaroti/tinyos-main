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
* - Neither the name of the University of Szeged nor the names of its
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
*/

/**
* @author Andras Becsi (abecsi@inf.u-szeged.hu)
*/

#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QReadWriteLock>
#include <QFile>
#include <QTimer>
#include "ClockWidget.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <stdio.h>

#include "configure.h"


class PlotWidget: public QwtPlot
{
    Q_OBJECT
    private:
        int interval; // timer in ms
        QTimer* updateTimer;
        double timeScale[BUFFER_SIZE+1];
        double displayBuffer[BUFFER_SIZE+1];
        QList<double> incomingDataBuffer;
        QList<double> dataExchangeBuffer;
        int dataPosition;
        QwtPlotCurve* curve;
        QwtPlotMarker* marker;
        void alignScales();
public:
        PlotWidget(QWidget* parent = 0);
        bool isRunning();
        void selectFile(bool save = true);
        void clearAndInit();

    public slots:
        void setShowMarker(bool on = false);
        void fillExchangeBuffer();
        //void adjustTime(double);
        void incomingData(double);
        void incomingData(const QList<double>&);
        void sliderValueChanged(double, bool init =0);
        void setTimerInterval(double interval);
        bool saveData(const QString&);
        bool openData(const QString&);
        void clearDisplay();
        void advance();
        void pause();
        void resume();
        void reset();

    signals:
        void dataImported();
        void fieldChanged(double f);
        void sliderUpdateNeeded(double value, double size = 0);
};

#endif // PLOTWIDGET_H
