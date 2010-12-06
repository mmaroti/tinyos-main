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
* Author: Mikl�s Mar�ti
* Author: P�ter Ruzicska
*/

#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "StationaryCalibrationModule.h"
#include "PeriodicalCalibrationModule.h"
#include "TurntableCalibrationModule.h"

class Application;

namespace Ui {
    class CalibrationWidget;
}

class CalibrationWidget : public QWidget {
    Q_OBJECT
public:
    CalibrationWidget(QWidget *parent, Application &app);
    ~CalibrationWidget();

protected:
    void changeEvent(QEvent *e);
    //virtual void mousePressEvent(QMouseEvent * event);

private:
    Ui::CalibrationWidget *ui;
    Application &application;
    StationaryCalibrationModule *calibrationModule;
    PeriodicalCalibrationModule *periodicalCalibrationModule;
    TurntableCalibrationModule *turntableCalibrationModule;

    void loadCalibrationResults();

private slots:
    void on_startButton_clicked();
    void on_exportButton_clicked();
    void on_importButton_clicked();
    void on_clearButton_clicked();
    void on_useButton_clicked();
    void OnFileLoaded();

signals:
    void calibrationDone();
};

#endif // CALIBRATIONWIDGET_H
