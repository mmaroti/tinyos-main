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

/*void CalibrationWidget::mousePressEvent(QMouseEvent *event)
{
    QString instructions = "";
    if (event->buttons() & Qt::LeftButton) {
        if ( ui->stationaryButton->isChecked() ) {
            instructions.append("Stationary Calibration\n\n");
            instructions.append("Please make a record the following way: \n");
            instructions.append("1) put down the mote on a stable horizontal surface and leave it idle for at least 3 seconds, \n");
            instructions.append("2) pick up the mote and put it back on the opposite side, \n");
            instructions.append("3) repeat 1) and 2) for all 3 side-pairs.");
        } else if ( ui->periodicalButton->isChecked() ) {
            instructions.append("Periodical Calibration\n\n");
            instructions.append("Please make a record the following way: \n");
            instructions.append("1) put down the mote on a stable horizontal surface and leave it idle for at least 3 seconds, \n");
            instructions.append("2) pick up the mote and put it back on the opposite side, \n");
            instructions.append("3) repeat 1) and 2) for all 3 side-pairs.");
        } else if ( ui->turntableButton->isChecked() ) {
            instructions.append("Turntable Calibration\n\n");
            instructions.append("NOTE: you will need a turntable for this calibration! \n\n");
            instructions.append("Please make a record the following way: \n");
            instructions.append("1) Set your turntable to 45RPMs, adjust rotation speed if necesary, \n");
            instructions.append("2) put down the mote on a stable horizontal surface and leave it idle for at least 3 seconds, \n");
            instructions.append("3) pick up the mote and put it on your spinning turntable. You can turn it off while you place your mote on it, then turn it back on. Leave it spinning for at least 3 seconds, \n");
            instructions.append("4) pick up your mote and place it back on the turntable on its opposite side, ");
            instructions.append("5) repeat 3) and 4) for all 3 side-pairs.");
        } else {
            instructions = "Please select a module!";
        }

        ui->instructionsText->setText(instructions);
    }
}*/

void CalibrationWidget::on_startButton_clicked()
{
    //QMessageBox msgBox;
    QString message = "";
    if ( ui->stationaryButton->isChecked() ) {

        message = calibrationModule->Calibrate();
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        application.dataRecorder.loadCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else if ( ui->periodicalButton->isChecked() ) {
        periodicalCalibrationModule->Calibrate("x");
        periodicalCalibrationModule->Calibrate("y");
        message.append(periodicalCalibrationModule->Calibrate("z"));
        message.append(periodicalCalibrationModule->SVD());
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        application.dataRecorder.loadCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else if ( ui->turntableButton->isChecked() ) {
        message = turntableCalibrationModule->Calibrate(45);
        application.showConsoleMessage(message);
        application.dataRecorder.saveCalibrationData();
        application.dataRecorder.loadCalibrationData();
        loadCalibrationResults();
        emit calibrationDone();
    } else {
        message = "Please select a module!";
    }

    message.clear();
    calibrationModule->clearWindows();
    calibrationModule->clearIdleSides();
}

void CalibrationWidget::on_exportButton_clicked()
{
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        application.dataRecorder.saveCalibToFile( fn );
    }
}

void CalibrationWidget::on_importButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select a file to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.dataRecorder.loadCalibFromFile( file );;
    }
    loadCalibrationResults();
}

void CalibrationWidget::on_clearButton_clicked()
{
    application.dataRecorder.clearCalibrationData();
    loadCalibrationResults();
}

void CalibrationWidget::on_useButton_clicked()
{
    application.dataRecorder.loadCalibrationData();
    loadCalibrationResults();
    emit calibrationDone();
}

void CalibrationWidget::OnFileLoaded()
{
    loadCalibrationResults();
}

void CalibrationWidget::loadCalibrationResults()
{
    QString message = "";
    message.append("Calibration data stored in system: \n");
    message.append("\n Accelerometer Calibration Data: \n");

    for (int i = 0; i < 12; ++i) {
        message.append( QString::number(application.dataRecorder.getAccelCalibration()[i]) + "\n" );
    }

    message.append("\n Gyroscope Avarages Data: \n");
    for (int i = 0; i < 3; ++i) {
        message.append( QString::number(application.dataRecorder.getGyroMinAvgs()[i]) + "\n" );
    }

    message.append("\n Gyroscope Calibration Data: \n");
    for (int i = 0; i < 12; ++i) {
        message.append( QString::number(application.dataRecorder.getGyroCalibration()[i]) + "\n" );
    }   

    message.append("\nCalibration Data Stored in Registry: \n\n");
    message.append("stationaryCalibrationData: \n");
    if( application.settings.contains("stationaryCalibrationData/size") ){
        int size = application.settings.beginReadArray("stationaryCalibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            message.append( QString::number(application.settings.value("stationaryCalibrationData").toDouble()) + "\n");
        }
        application.settings.endArray();
    } else {
        message.append("no data\n");
    }
    message.append("gyroAvgsData: \n");
    if(application.settings.contains("gyroAvgsData/size")){
        int size = application.settings.beginReadArray("gyroAvgsData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            message.append(QString::number(application.settings.value("gyroAvgsData").toDouble()) + "\n");
        }
        application.settings.endArray();
    } else {
        message.append("no data\n");
    }
    message.append("gyroCalibrationData: \n");
    if(application.settings.contains("gyroCalibrationData/size")){
        int size = application.settings.beginReadArray("gyroCalibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            message.append(QString::number(application.settings.value("gyroCalibrationData").toDouble()) + "\n");
        }
        application.settings.endArray();
    } else {
        message.append("no data\n");
    }
    ui->calibrationResults->setText(message);
}
