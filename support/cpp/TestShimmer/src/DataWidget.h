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

#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include "Data.hpp"
#include "DataPlot.h"
#include <QMainWindow>
#include <QTextEdit>
#include "Widget3D.h"

class Application;
class DataRecorder;

namespace ipo {
    class Results;
}

namespace Ui {

    class DataWidget;

}

class DataWidget : public QWidget {
    Q_OBJECT

public:
    DataWidget(QWidget *parent, Application &app);
    ~DataWidget();

protected:
        void changeEvent(QEvent *e);

private:

        DataPlot *plot;
        Ui::DataWidget *ui;
        Application &application;

        QTextEdit *textBox;
        Widget3D *widget3d;

        void setCheckBoxes(bool);
        bool showErrorGraphs;

signals:
        void SolverStarted();
        void SolverFinished();

private slots:
        //void on_temp_clicked();
        //void on_voltage_clicked();
        void on_rawGyroX_clicked();
        void on_rawGyroY_clicked();
        void on_rawGyroZ_clicked();
        void on_rawAccX_clicked();
        void on_rawAccY_clicked();
        void on_rawAccZ_clicked();
        void on_gyroX_clicked();
        void on_gyroY_clicked();
        void on_gyroZ_clicked();
        void on_accX_clicked();
        void on_accY_clicked();
        void on_accZ_clicked();
        void on_absAcc_clicked();
        void on_angXY_clicked();
        void on_angYZ_clicked();
        void on_angZX_clicked();

        void on_angX_clicked();
        void on_angY_clicked();
        void on_angZ_clicked();

        void on_eulerX_clicked();
        void on_eulerY_clicked();
        void on_eulerZ_clicked();

        void on_intX_clicked();
        void on_intY_clicked();
        void on_intZ_clicked();

        void on_corrX_clicked();
        void on_corrY_clicked();
        void on_corrZ_clicked();

        void on_rawAccBox_clicked();
        void on_accelBox_clicked();
        void on_rawGyroBox_clicked();
        void on_gyroBox_clicked();
        void on_anglesBox_clicked();
        void on_tiltAnglesBox_clicked();
        void on_eulerBox_clicked();
        void on_intBox_clicked();
        void on_corrBox_clicked();

        void on_saveBtn_clicked();
        void on_clearBtn_clicked();
        void on_recordBtn_clicked();
        void on_loadBtn_clicked();
        void on_refreshButton_clicked();
        void on_regressionButton_clicked();

        void on_presetsComboBox_currentIndexChanged();
        void on_showErrorGraphs_chB_clicked();

        void newCalibrationOccured();
        void onTrim();
        void onCopy();
        void onCut();

        void finished(bool error, const char* msg, const ipo::Results* res);

        void onCalculateRange(int from, int to);
};

#endif // DATAWIDGET_H
