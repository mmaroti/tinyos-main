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
#include "DataWidget.h"
#include "ui_DataWidget.h"
#include <qfiledialog.h>
#include "QtDebug"
#include "window.h"

DataWidget::DataWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::DataWidget),
    application(app)
{
        ui->setupUi(this);
        plot = new DataPlot(ui->scrollArea, app);
        ui->scrollArea->setWidget(plot);
        //connect(&plot, SIGNAL(angleChanged(double)), Window::currentGlWidget , SLOT(onAngleChanged(double)) );

        gyroMinAvgs = application.dataRecorder.getGyroMinAvgs();
        gyroCalibrationData = application.dataRecorder.getGyroCalibration();
        accelCalibrationData = application.dataRecorder.getAccelCalibration();

}

DataWidget::~DataWidget()
{
        delete ui;
}

void DataWidget::changeEvent(QEvent *e)
{
        QWidget::changeEvent(e);
        switch (e->type())
        {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void DataWidget::newCalibrationOccured()
{
    plot->setGraphs(DataPlot::CALIB, true);
    plot->onNewCalibration();
}

void DataWidget::on_recordBtn_clicked()
{
    QString text = ui->recordBtn->text();
    if( text == "(R)ecord" )
    {
            ui->recordBtn->setText("S(t)op");
            ui->recordBtn->setShortcut(Qt::Key_T);
            connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
    }
    else
    {
            ui->recordBtn->setText("(R)ecord");
            ui->recordBtn->setShortcut(Qt::Key_R);
            disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
    }
}

void DataWidget::on_clearBtn_clicked()
{
    plot->setGraphs(DataPlot::CALIB, false);
    application.dataRecorder.clearMessages();
}

void DataWidget::on_loadBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/");
    if ( !file.isEmpty() ) {
        application.dataRecorder.loadSamples( file );
    }
    //plot->DataPlot::onSampleAdded();
}

void DataWidget::on_exportBtn_clicked()
{
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        application.dataRecorder.saveSamples( fn );

        QFile f( fn );

        if( !f.open( QIODevice::Append ) )
          {
              return;
          }

        QTextStream ts( &f );
        double xtemp, ytemp, ztemp, avgtemp, xyAngle, yzAngle, zxAngle, xGyro, yGyro, zGyro;

        ts << "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" << endl;
        for (int i = 0; i < application.dataRecorder.size(); i++) {
            xtemp = application.dataRecorder.at(i).xAccel * accelCalibrationData[0] + application.dataRecorder.at(i).yAccel * accelCalibrationData[1] + application.dataRecorder.at(i).zAccel * accelCalibrationData[2] + accelCalibrationData[9];
            ytemp = application.dataRecorder.at(i).xAccel * accelCalibrationData[3] + application.dataRecorder.at(i).yAccel * accelCalibrationData[4] + application.dataRecorder.at(i).zAccel * accelCalibrationData[5] + accelCalibrationData[10];
            ztemp = application.dataRecorder.at(i).xAccel * accelCalibrationData[6] + application.dataRecorder.at(i).yAccel * accelCalibrationData[7] + application.dataRecorder.at(i).zAccel * accelCalibrationData[8] + accelCalibrationData[11];
            avgtemp = sqrt( pow(xtemp, 2.0) + pow(ytemp, 2.0) + pow(ztemp, 2.0) );
            xyAngle = plot->calculateAngle(xtemp,ytemp);
            yzAngle = plot->calculateAngle(ytemp,ztemp);
            zxAngle = plot->calculateAngle(ztemp,xtemp);
            xGyro = (application.dataRecorder.at(i).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[0] + (application.dataRecorder.at(i).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[1] + (application.dataRecorder.at(i).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[2];
            yGyro = (application.dataRecorder.at(i).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[3] + (application.dataRecorder.at(i).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[4] + (application.dataRecorder.at(i).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[5];
            zGyro = (application.dataRecorder.at(i).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[6] + (application.dataRecorder.at(i).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[7] + (application.dataRecorder.at(i).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[8];

            ts << xtemp << "," << ytemp << "," << ztemp << "," << avgtemp << "," << xyAngle << "," << yzAngle << "," << zxAngle << "," << xGyro << "," << yGyro << "," << zGyro << endl;
        }

        ts << "#Static Calibration Data" << endl;
        int size = application.settings.beginReadArray("calibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            ts << application.settings.value("calibrationData").toString() + "\n" ;
        }
        application.settings.endArray();

        ts << "#Gyro Calibration Data" << endl;
        size = application.settings.beginReadArray("gyroCalibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            ts << application.settings.value("gyroCalibrationData").toString() + "\n" ;
        }
        application.settings.endArray();

        ts.flush();
        f.close();
    }
}

void DataWidget::on_rawAccX_clicked()
{
        plot->setGraphs(DataPlot::XRAWACC, ui->rawAccX->checkState());
}

void DataWidget::on_rawAccY_clicked()
{
        plot->setGraphs(DataPlot::YRAWACC, ui->rawAccY->checkState());
}

void DataWidget::on_rawAccZ_clicked()
{
        plot->setGraphs(DataPlot::ZRAWACC, ui->rawAccZ->checkState());
}

void DataWidget::on_rawGyroX_clicked()
{
        plot->setGraphs(DataPlot::XRAWGYRO, ui->rawGyroX->checkState());
}

void DataWidget::on_rawGyroY_clicked()
{
        plot->setGraphs(DataPlot::YRAWGYRO, ui->rawGyroY->checkState());
}

void DataWidget::on_rawGyroZ_clicked()
{
        plot->setGraphs(DataPlot::ZRAWGYRO, ui->rawGyroZ->checkState());
}

/*void DataWidget::on_voltage_clicked()
{
        plot->setGraphs(DataPlot::VOLTAGE, ui->voltage->checkState());
}

void DataWidget::on_temp_clicked()
{
        plot->setGraphs(DataPlot::TEMP, ui->temp->checkState());
}*/

void DataWidget::on_accX_clicked()
{
    plot->setGraphs(DataPlot::XACC, ui->accX->checkState());
}

void DataWidget::on_accY_clicked()
{
    plot->setGraphs(DataPlot::YACC, ui->accY->checkState());
}

void DataWidget::on_accZ_clicked()
{
    plot->setGraphs(DataPlot::ZACC, ui->accZ->checkState());
}

void DataWidget::on_absAcc_clicked()
{
    plot->setGraphs(DataPlot::ABSACC, ui->absAcc->checkState());
}

void DataWidget::on_angXY_clicked()
{
    plot->setGraphs(DataPlot::XYANG, ui->angXY->checkState());
}

void DataWidget::on_angYZ_clicked()
{
    plot->setGraphs(DataPlot::YZANG, ui->angYZ->checkState());
}

void DataWidget::on_angZX_clicked()
{
    plot->setGraphs(DataPlot::ZXANG, ui->angZX->checkState());
}

void DataWidget::on_gyroX_clicked()
{
    plot->setGraphs(DataPlot::XGYRO, ui->gyroX->checkState());
}

void DataWidget::on_gyroY_clicked()
{
    plot->setGraphs(DataPlot::YGYRO, ui->gyroY->checkState());
}

void DataWidget::on_gyroZ_clicked()
{
    plot->setGraphs(DataPlot::ZGYRO, ui->gyroZ->checkState());
}

void DataWidget::on_angX_clicked()
{
    plot->setGraphs(DataPlot::XANG, ui->angX->checkState());
}

void DataWidget::on_angY_clicked()
{
    plot->setGraphs(DataPlot::YANG, ui->angY->checkState());
}

void DataWidget::on_angZ_clicked()
{
    plot->setGraphs(DataPlot::ZANG, ui->angZ->checkState());
}

void DataWidget::on_rawAccBox_clicked()
{
    if(ui->rawAccBox->isChecked()){
        ui->rawAccX->setChecked(true);
        ui->rawAccY->setChecked(true);
        ui->rawAccZ->setChecked(true);
    } else {
        ui->rawAccX->setChecked(false);
        ui->rawAccY->setChecked(false);
        ui->rawAccZ->setChecked(false);
    }
    on_rawAccX_clicked();
    on_rawAccY_clicked();
    on_rawAccZ_clicked();
}

void DataWidget::on_accelBox_clicked()
{
    if(ui->accelBox->isChecked()){
        ui->accX->setChecked(true);
        ui->accY->setChecked(true);
        ui->accZ->setChecked(true);
    } else {
        ui->accX->setChecked(false);
        ui->accY->setChecked(false);
        ui->accZ->setChecked(false);
    }
    on_accX_clicked();
    on_accY_clicked();
    on_accZ_clicked();
}

void DataWidget::on_gyroBox_clicked()
{
    if(ui->gyroBox->isChecked()){
        ui->gyroX->setChecked(true);
        ui->gyroY->setChecked(true);
        ui->gyroZ->setChecked(true);
    } else {
        ui->gyroX->setChecked(false);
        ui->gyroY->setChecked(false);
        ui->gyroZ->setChecked(false);
    }
    on_gyroX_clicked();
    on_gyroY_clicked();
    on_gyroZ_clicked();
}

void DataWidget::on_rawGyroBox_clicked()
{
    if(ui->rawGyroBox->isChecked()){
        ui->rawGyroX->setChecked(true);
        ui->rawGyroY->setChecked(true);
        ui->rawGyroZ->setChecked(true);
    } else {
        ui->rawGyroX->setChecked(false);
        ui->rawGyroY->setChecked(false);
        ui->rawGyroZ->setChecked(false);
    }
    on_rawGyroX_clicked();
    on_rawGyroY_clicked();
    on_rawGyroZ_clicked();
}


void DataWidget::on_anglesBox_clicked()
{
    if(ui->anglesBox->isChecked()){
        ui->angXY->setChecked(true);
        ui->angYZ->setChecked(true);
        ui->angZX->setChecked(true);
    } else {
        ui->angXY->setChecked(false);
        ui->angYZ->setChecked(false);
        ui->angZX->setChecked(false);
    }
    on_angXY_clicked();
    on_angYZ_clicked();
    on_angZX_clicked();
}

void DataWidget::on_angles2Box_clicked()
{
    if(ui->angles2Box->isChecked()){
        ui->angX->setChecked(true);
        ui->angY->setChecked(true);
        ui->angZ->setChecked(true);
    } else {
        ui->angX->setChecked(false);
        ui->angY->setChecked(false);
        ui->angZ->setChecked(false);
    }
    on_angX_clicked();
    on_angY_clicked();
    on_angZ_clicked();
}
