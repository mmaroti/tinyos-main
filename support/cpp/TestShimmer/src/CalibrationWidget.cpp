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

#include "CalibrationWidget.h"
#include "ui_CalibrationWidget.h"
#include "DataRecorder.h"
#include "StationaryCalibrationModule.h"
#include "PeriodicalCalibrationModule.h"
#include "Application.h"
#include <qfiledialog.h>

CalibrationWidget::CalibrationWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget),
    application(app)
{
    ui->setupUi(this);

    calibrationModule = new StationaryCalibrationModule(app);
    periodicalCalibrationModule = new PeriodicalCalibrationModule(app, *calibrationModule);
    turntableCalibrationModule = new TurntableCalibrationModule(app);

    application.dataRecorder.loadCalibrationData();
    loadCalibrationResults();
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
	ui->retranslateUi(this);
	break;
    default:
	break;
    }
}

void CalibrationWidget::on_startButton_clicked()
{
    QString message = "";
    if ( ui->stationaryButton->isChecked() ) {

        message = calibrationModule->Calibrate();
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else if ( ui->periodicalButton->isChecked() ) {
        periodicalCalibrationModule->Calibrate("x");
        periodicalCalibrationModule->Calibrate("y");
        message.append(periodicalCalibrationModule->Calibrate("z"));
        message.append(periodicalCalibrationModule->SVD());
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else if ( ui->turntableButton->isChecked() ) {
        message = turntableCalibrationModule->Calibrate(45);
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else {
        message = "Please select a module!";
    }

    message.clear();
    calibrationModule->clearWindows();
    calibrationModule->clearIdleSides();
}

void CalibrationWidget::loadCalibrationResults()
{
    QString message = "";
    message.append("Calibration data stored in system: \n");
    message.append("\n Accelerometer Calibration Data: \n");

    for (int i = 0; i < 12; ++i) {
        message.append( QString::number(application.dataRecorder.getAccelCalibration()[i]) + "\n" );
    }

    message.append("\n Gyroscope Calibration Data: \n");
    for (int i = 0; i < 12; ++i) {
        message.append( QString::number(application.dataRecorder.getGyroCalibration()[i]) + "\n" );
    }

    message.append("\n Gyroscope Avarages Data: \n");
    for (int i = 0; i < 3; ++i) {
        message.append( QString::number(application.dataRecorder.getGyroMinAvgs()[i]) + "\n" );
    }

    ui->calibrationResults->setText(message);
    application.dataRecorder.loadCalibrationData();
}
