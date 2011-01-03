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
* Author: Miklï¿½s Marï¿½ti
* Author: Pï¿½ter Ruzicska
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

        textBox = new QTextEdit(this);
        textBox->setReadOnly(true);
        ui->menuLayout->insertWidget(3,textBox);
        textBox->hide();
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
            connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);
            ui->regressionButton->setEnabled(false);
    }
    else
    {
            ui->recordBtn->setText("(R)ecord");
            ui->recordBtn->setShortcut(Qt::Key_R);
            disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
            ui->regressionButton->setEnabled(true);
    }
}

void DataWidget::on_clearBtn_clicked()
{
    plot->setGraphs(DataPlot::CALIB, false);
    application.dataRecorder.clearSamples();
    application.dataRecorder.loadCalibrationData();
}

void DataWidget::on_refreshButton_clicked()
{
    plot->update();
}

void DataWidget::on_loadBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.dataRecorder.loadSamples( file );
        application.dataRecorder.loadCalibFromFileToLive( file );
    }
}

void DataWidget::on_saveBtn_clicked()
{
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        application.dataRecorder.saveSamples( fn );
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

void DataWidget::on_eulerX_clicked()
{
    plot->setGraphs(DataPlot::XEUL, ui->eulerX->checkState());
}

void DataWidget::on_eulerY_clicked()
{
    plot->setGraphs(DataPlot::YEUL, ui->eulerY->checkState());
}

void DataWidget::on_eulerZ_clicked()
{
    plot->setGraphs(DataPlot::ZEUL, ui->eulerZ->checkState());
}

void DataWidget::on_intX_clicked()
{
    plot->setGraphs(DataPlot::XINT, ui->intX->checkState());
}

void DataWidget::on_intY_clicked()
{
    plot->setGraphs(DataPlot::YINT, ui->intY->checkState());
}

void DataWidget::on_intZ_clicked()
{
    plot->setGraphs(DataPlot::ZINT, ui->intZ->checkState());
}

void DataWidget::on_corrX_clicked()
{
    plot->setGraphs(DataPlot::XCORRANG, ui->corrX->checkState());
}

void DataWidget::on_corrY_clicked()
{
    plot->setGraphs(DataPlot::YCORRANG, ui->corrY->checkState());
}

void DataWidget::on_corrZ_clicked()
{
    plot->setGraphs(DataPlot::ZCORRANG, ui->corrZ->checkState());
}

void DataWidget::on_rawAccBox_clicked()
{
    if(ui->rawAccBox->isChecked()){
        if(!ui->rawAccX->checkState() && !ui->rawAccY->checkState() && !ui->rawAccZ->checkState()){
            ui->rawAccX->setChecked(true);
            ui->rawAccY->setChecked(true);
            ui->rawAccZ->setChecked(true);
        }
        on_rawAccX_clicked();
        on_rawAccY_clicked();
        on_rawAccZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XRAWACC, false);
        plot->setGraphs(DataPlot::YRAWACC, false);
        plot->setGraphs(DataPlot::ZRAWACC, false);
    }
}

void DataWidget::on_accelBox_clicked()
{
    if(ui->accelBox->isChecked()){
        if(!ui->accX->checkState() && !ui->accY->checkState() && !ui->accZ->checkState() && !ui->absAcc->checkState()){
            ui->accX->setChecked(true);
            ui->accY->setChecked(true);
            ui->accZ->setChecked(true);
            ui->absAcc->setChecked(true);
        }
        on_accX_clicked();
        on_accY_clicked();
        on_accZ_clicked();
        on_absAcc_clicked();
    } else {
        plot->setGraphs(DataPlot::XACC, false);
        plot->setGraphs(DataPlot::YACC, false);
        plot->setGraphs(DataPlot::ZACC, false);
        plot->setGraphs(DataPlot::ABSACC, false);
    }
}

void DataWidget::on_gyroBox_clicked()
{
    if(ui->gyroBox->isChecked()){
        if(!ui->gyroX->checkState() && !ui->gyroY->checkState() && !ui->gyroZ->checkState()){
            ui->gyroX->setChecked(true);
            ui->gyroY->setChecked(true);
            ui->gyroZ->setChecked(true);
        }
        on_gyroX_clicked();
        on_gyroY_clicked();
        on_gyroZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XGYRO, false);
        plot->setGraphs(DataPlot::YGYRO, false);
        plot->setGraphs(DataPlot::ZGYRO, false);
    }
}

void DataWidget::on_rawGyroBox_clicked()
{
    if(ui->rawGyroBox->isChecked()){
        if(!ui->rawGyroX->checkState() && !ui->rawGyroY->checkState() && !ui->rawGyroZ->checkState()){
            ui->rawGyroX->setChecked(true);
            ui->rawGyroY->setChecked(true);
            ui->rawGyroZ->setChecked(true);
        }
        on_rawGyroX_clicked();
        on_rawGyroY_clicked();
        on_rawGyroZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XRAWGYRO, false);
        plot->setGraphs(DataPlot::YRAWGYRO, false);
        plot->setGraphs(DataPlot::ZRAWGYRO, false);
    }
}


void DataWidget::on_anglesBox_clicked()
{
    if(ui->anglesBox->isChecked()){
        if(!ui->angXY->checkState() && !ui->angYZ->checkState() && !ui->angZX->checkState()){
            ui->angXY->setChecked(true);
            ui->angYZ->setChecked(true);
            ui->angZX->setChecked(true);
        }
        on_angXY_clicked();
        on_angYZ_clicked();
        on_angZX_clicked();
    } else {
        plot->setGraphs(DataPlot::XYANG, false);
        plot->setGraphs(DataPlot::YZANG, false);
        plot->setGraphs(DataPlot::ZXANG, false);
    }
}

void DataWidget::on_tiltAnglesBox_clicked()
{
    if(ui->tiltAnglesBox->isChecked()){
        if(!ui->angX->checkState() && !ui->angY->checkState() && !ui->angZ->checkState()){
            ui->angX->setChecked(true);
            ui->angY->setChecked(true);
            ui->angZ->setChecked(true);
        }
        on_angX_clicked();
        on_angY_clicked();
        on_angZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XANG, false);
        plot->setGraphs(DataPlot::YANG, false);
        plot->setGraphs(DataPlot::ZANG, false);
    }
}

void DataWidget::on_eulerBox_clicked()
{
    if(ui->eulerBox->isChecked()){
        if(!ui->eulerX->checkState() && !ui->eulerY->checkState() && !ui->eulerZ->checkState()){
            ui->eulerX->setChecked(true);
            ui->eulerY->setChecked(true);
            ui->eulerZ->setChecked(true);
        }
        on_eulerX_clicked();
        on_eulerY_clicked();
        on_eulerZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XEUL, false);
        plot->setGraphs(DataPlot::YEUL, false);
        plot->setGraphs(DataPlot::ZEUL, false);
    }
}

void DataWidget::on_intBox_clicked()
{
    if(ui->intBox->isChecked()){
        if(!ui->intX->checkState() && !ui->intY->checkState() && !ui->intZ->checkState()){
            ui->intX->setChecked(true);
            ui->intY->setChecked(true);
            ui->intZ->setChecked(true);
        }
        on_intX_clicked();
        on_intY_clicked();
        on_intZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XINT, false);
        plot->setGraphs(DataPlot::YINT, false);
        plot->setGraphs(DataPlot::ZINT, false);
    }
}

void DataWidget::on_corrBox_clicked()
{
    if(ui->corrBox->isChecked()){
        if(!ui->corrX->checkState() && !ui->corrY->checkState() && !ui->corrZ->checkState()){
            ui->corrX->setChecked(true);
            ui->corrY->setChecked(true);
            ui->corrZ->setChecked(true);
        }
        on_corrX_clicked();
        on_corrY_clicked();
        on_corrZ_clicked();
    } else {
        plot->setGraphs(DataPlot::XCORRANG, false);
        plot->setGraphs(DataPlot::YCORRANG, false);
        plot->setGraphs(DataPlot::ZCORRANG, false);
    }
}

void DataWidget::onTrim()
{
    application.dataRecorder.edit("trim");
    plot->getStartPos().setX(0);
    plot->getLastPos().setX(0);
    plot->update();
}

void DataWidget::onCopy()
{
    application.dataRecorder.edit("copy");
    plot->getStartPos().setX(0);
    plot->getLastPos().setX(0);
    plot->update();
}

void DataWidget::onCut()
{
    application.dataRecorder.edit("cut");
    plot->getStartPos().setX(0);
    plot->getLastPos().setX(0);
    plot->update();
}

void DataWidget::finished(bool error, const char* msg, const ipo::Results* res) {

    if (!error) {
        application.dataRecorder.loadResults(res);
        bool on = true;
        int graphs = DataPlot::XCORRANG | DataPlot::YCORRANG | DataPlot::ZCORRANG ;
        plot->setGraphs(graphs, on);
        ui->corrX->setChecked(true);
        ui->corrY->setChecked(true);
        ui->corrZ->setChecked(true);
        ui->corrBox->setChecked(true);
        plot->update();
    }
    else {

        QMessageBox mbox;
        mbox.setText(msg);
        mbox.exec();
    }

    ui->loadBtn->setEnabled(true);
    ui->regressionButton->setEnabled(true);
    ui->recordBtn->setEnabled(true);
    ui->clearBtn->setEnabled(true);

    emit SolverFinished();
}

void DataWidget::on_regressionButton_clicked()
{
    static bool registered(false);

    if(!registered) {
        registered = true;

        QObject::connect(&(application.solver), SIGNAL(solver_done(bool , const char* , const ipo::Results* )),
                         this, SLOT(  finished(bool , const char* , const ipo::Results* )), Qt::DirectConnection);
    }

    // TODO If no samples are loaded, disable regression button

    application.dataRecorder.dump_calibration_data();

    bool error = application.solver.start();

    if(!error){
        ui->loadBtn->setEnabled(false);
        ui->regressionButton->setEnabled(false);
        ui->recordBtn->setEnabled(false);
        ui->clearBtn->setEnabled(false);

        emit SolverStarted();
    }
}

void DataWidget::on_presetsComboBox_currentIndexChanged()
{
    if(ui->presetsComboBox->currentIndex() == 0){
        on_rawAccX_clicked();
        on_rawAccY_clicked();
        on_rawAccZ_clicked();
        on_accX_clicked();
        on_accY_clicked();
        on_accZ_clicked();
        on_absAcc_clicked();
        on_gyroX_clicked();
        on_gyroY_clicked();
        on_gyroZ_clicked();
        on_rawGyroX_clicked();
        on_rawGyroY_clicked();
        on_rawGyroZ_clicked();
        on_angXY_clicked();
        on_angYZ_clicked();
        on_angZX_clicked();
        on_angX_clicked();
        on_angY_clicked();
        on_angZ_clicked();
        on_eulerX_clicked();
        on_eulerY_clicked();
        on_eulerZ_clicked();
        on_corrX_clicked();
        on_corrY_clicked();
        on_corrZ_clicked();
        textBox->hide();
    } else if(ui->presetsComboBox->currentIndex() == 1){
        plot->setGraphs(DataPlot::XRAWACC, false);
        plot->setGraphs(DataPlot::YRAWACC, false);
        plot->setGraphs(DataPlot::ZRAWACC, false);
        plot->setGraphs(DataPlot::XACC, false);
        plot->setGraphs(DataPlot::YACC, false);
        plot->setGraphs(DataPlot::ZACC, false);
        plot->setGraphs(DataPlot::ABSACC, false);
        plot->setGraphs(DataPlot::XGYRO, false);
        plot->setGraphs(DataPlot::YGYRO, false);
        plot->setGraphs(DataPlot::ZGYRO, false);
        plot->setGraphs(DataPlot::XRAWGYRO, false);
        plot->setGraphs(DataPlot::YRAWGYRO, false);
        plot->setGraphs(DataPlot::ZRAWGYRO, false);
        plot->setGraphs(DataPlot::XYANG, false);
        plot->setGraphs(DataPlot::YZANG, false);
        plot->setGraphs(DataPlot::ZXANG, false);
        plot->setGraphs(DataPlot::XANG, false);
        plot->setGraphs(DataPlot::YANG, false);
        plot->setGraphs(DataPlot::ZANG, false);
        plot->setGraphs(DataPlot::XEUL, false);
        plot->setGraphs(DataPlot::YEUL, false);
        plot->setGraphs(DataPlot::ZEUL, false);
        plot->setGraphs(DataPlot::XINT, false);
        plot->setGraphs(DataPlot::YINT, false);
        plot->setGraphs(DataPlot::ZINT, false);
        plot->setGraphs(DataPlot::XCORRANG, true);
        plot->setGraphs(DataPlot::YCORRANG, false);
        plot->setGraphs(DataPlot::ZCORRANG, true);
        textBox->setText("Piros: alkari szupináció\nKék: könyök flexió");
        textBox->show();
    } else if(ui->presetsComboBox->currentIndex() == 2){
        plot->setGraphs(DataPlot::XRAWACC, false);
        plot->setGraphs(DataPlot::YRAWACC, false);
        plot->setGraphs(DataPlot::ZRAWACC, false);
        plot->setGraphs(DataPlot::XACC, false);
        plot->setGraphs(DataPlot::YACC, false);
        plot->setGraphs(DataPlot::ZACC, false);
        plot->setGraphs(DataPlot::ABSACC, false);
        plot->setGraphs(DataPlot::XGYRO, false);
        plot->setGraphs(DataPlot::YGYRO, false);
        plot->setGraphs(DataPlot::ZGYRO, false);
        plot->setGraphs(DataPlot::XRAWGYRO, false);
        plot->setGraphs(DataPlot::YRAWGYRO, false);
        plot->setGraphs(DataPlot::ZRAWGYRO, false);
        plot->setGraphs(DataPlot::XYANG, false);
        plot->setGraphs(DataPlot::YZANG, false);
        plot->setGraphs(DataPlot::ZXANG, false);
        plot->setGraphs(DataPlot::XANG, false);
        plot->setGraphs(DataPlot::YANG, false);
        plot->setGraphs(DataPlot::ZANG, false);
        plot->setGraphs(DataPlot::XEUL, false);
        plot->setGraphs(DataPlot::YEUL, false);
        plot->setGraphs(DataPlot::ZEUL, false);
        plot->setGraphs(DataPlot::XINT, false);
        plot->setGraphs(DataPlot::YINT, false);
        plot->setGraphs(DataPlot::ZINT, false);
        plot->setGraphs(DataPlot::XCORRANG, false);
        plot->setGraphs(DataPlot::YCORRANG, true);
        plot->setGraphs(DataPlot::ZCORRANG, false);
        textBox->setText("Zöld: rotáció");
        textBox->show();
    } else if(ui->presetsComboBox->currentIndex() == 3){
        plot->setGraphs(DataPlot::XRAWACC, false);
        plot->setGraphs(DataPlot::YRAWACC, false);
        plot->setGraphs(DataPlot::ZRAWACC, false);
        plot->setGraphs(DataPlot::XACC, true);
        plot->setGraphs(DataPlot::YACC, true);
        plot->setGraphs(DataPlot::ZACC, true);
        plot->setGraphs(DataPlot::ABSACC, false);
        plot->setGraphs(DataPlot::XGYRO, false);
        plot->setGraphs(DataPlot::YGYRO, false);
        plot->setGraphs(DataPlot::ZGYRO, false);
        plot->setGraphs(DataPlot::XRAWGYRO, false);
        plot->setGraphs(DataPlot::YRAWGYRO, false);
        plot->setGraphs(DataPlot::ZRAWGYRO, false);
        plot->setGraphs(DataPlot::XYANG, false);
        plot->setGraphs(DataPlot::YZANG, false);
        plot->setGraphs(DataPlot::ZXANG, false);
        plot->setGraphs(DataPlot::XANG, false);
        plot->setGraphs(DataPlot::YANG, false);
        plot->setGraphs(DataPlot::ZANG, false);
        plot->setGraphs(DataPlot::XEUL, false);
        plot->setGraphs(DataPlot::YEUL, false);
        plot->setGraphs(DataPlot::ZEUL, false);
        plot->setGraphs(DataPlot::XINT, false);
        plot->setGraphs(DataPlot::YINT, false);
        plot->setGraphs(DataPlot::ZINT, false);
        plot->setGraphs(DataPlot::XCORRANG, false);
        plot->setGraphs(DataPlot::YCORRANG, false);
        plot->setGraphs(DataPlot::ZCORRANG, false);
        textBox->hide();
    }
}
