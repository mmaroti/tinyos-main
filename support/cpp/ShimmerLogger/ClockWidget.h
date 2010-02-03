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

#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QLabel>
#include <QBasicTimer>
#include <QTime>
#include <qglobal.h>

#include "configure.h"

class SimpleTime : QTime
{
    public:
        SimpleTime() : QTime(){
            setHMS(0,0,0);
        }
        void set(const QTime& time) {
            setHMS(time.hour(), time.minute(), time.second(), time.msec());
        }
        void reset(int h=0, int m=0, int s=0, int ms=0){
            setHMS(h, m, s, ms);
        }
        void add(int ms) {
            QTime tmp = addMSecs(ms);
            setHMS(tmp.hour(), tmp.minute(), tmp.second(), tmp.msec());
        }
        const QString toString() {
            return QString("%1:<font size=\"50\">%2</font>:<font size=\"50\">%3</font>.<font size=\"5\">%4</font>")
            .arg(hour(), 2, 10, QChar('0'))
            .arg(minute(),2, 10, QChar('0'))
            .arg(second(),2, 10, QChar('0'))
            .arg(msec()/GRANULARITY);
        }
};

class ClockWidget : public QLabel
{
    Q_OBJECT
    private:
        QBasicTimer updateTimer;
        SimpleTime internalTime;

        void timerEvent(QTimerEvent*);
        void setDisplay(const QString);
    public:
        ClockWidget(QWidget* parent=0);
        bool isRunning();
    public slots:
        void start();
        void start(const QTime& startTime);
        void stop();
        void reset();
};

#endif // CLOCKWIDGET_H
