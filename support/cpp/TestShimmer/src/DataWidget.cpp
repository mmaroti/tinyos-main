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
//	ui->recordBtn->setText("Record");
//	disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));

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
        ts << "===CALIBRATED DATA===" << endl;
        ts << "Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" << endl;
        for (int i = 0; i < application.dataRecorder.size(); i++) {
            xtemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(0) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(1) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(2) + plot->calibrationDataAt(9);
            ytemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(3) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(4) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(5) + plot->calibrationDataAt(10);
            ztemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(6) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(7) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(8) + plot->calibrationDataAt(11);
            avgtemp = sqrt( pow(xtemp, 2.0) + pow(ytemp, 2.0) + pow(ztemp, 2.0) );
            xyAngle = plot->calculateAngle(xtemp,ytemp);
            yzAngle = plot->calculateAngle(ytemp,ztemp);
            zxAngle = plot->calculateAngle(ztemp,xtemp);
            xGyro = (application.dataRecorder.at(i).xGyro - plot->gyroMinAvgsAt(0)) * plot->gyroCalibrationDataAt(0) + (application.dataRecorder.at(i).yGyro - plot->gyroMinAvgsAt(1)) * plot->gyroCalibrationDataAt(1) + (application.dataRecorder.at(i).zGyro - plot->gyroMinAvgsAt(2)) * plot->gyroCalibrationDataAt(2);
            yGyro = (application.dataRecorder.at(i).xGyro - plot->gyroMinAvgsAt(0)) * plot->gyroCalibrationDataAt(3) + (application.dataRecorder.at(i).yGyro - plot->gyroMinAvgsAt(1)) * plot->gyroCalibrationDataAt(4) + (application.dataRecorder.at(i).zGyro - plot->gyroMinAvgsAt(2)) * plot->gyroCalibrationDataAt(5);
            zGyro = (application.dataRecorder.at(i).xGyro - plot->gyroMinAvgsAt(0)) * plot->gyroCalibrationDataAt(6) + (application.dataRecorder.at(i).yGyro - plot->gyroMinAvgsAt(1)) * plot->gyroCalibrationDataAt(7) + (application.dataRecorder.at(i).zGyro - plot->gyroMinAvgsAt(2)) * plot->gyroCalibrationDataAt(8);

            ts << xtemp << "," << ytemp << "," << ztemp << "," << avgtemp << "," << xyAngle << "," << yzAngle << "," << zxAngle << "," << xGyro << "," << yGyro << "," << zGyro << endl;
        }

        int size = application.settings.beginReadArray("calibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);
            ts << application.settings.value("calibrationData").toString() + "\n" ;
        }
        application.settings.endArray();

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
