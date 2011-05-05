/* Copyright (c) 2011, University of Szeged
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
*      Author: Ali Baharev
*/

#ifndef ELLIPSOIDCALIBRATION_HPP
#define ELLIPSOIDCALIBRATION_HPP

#include "ui_EllipsoidCalibration.h"
#include "AccelMagSample.hpp"

class QTextStream;

class EllipsoidCalibration : public QWidget, private Ui::EllipsoidCalibration
{
    Q_OBJECT

public:

    EllipsoidCalibration(QWidget* parent);

public slots:

    void onNewSampleReceived(const AccelMagSample sample);

private slots:

    void on_tableWidget_itemActivated(QTableWidgetItem* item);
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_clearButton_clicked();
    void on_saveButton_clicked();
    void on_captureButton_pressed();

private:

    enum Column { ACCEL, MAGN, TEMP };

    void addItem(Column col, const QString& str);

    void clearAll();
    void clearSamples();
    void clearRows();
    void deleteRow(int row);

    void exportSamples(QTextStream& out);

    AccelMagSample previous;
    AccelMagSample current;
};

#endif // ELLIPSOIDCALIBRATION_HPP
