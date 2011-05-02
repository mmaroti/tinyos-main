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

#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include "EllipsoidCalibration.hpp"

using namespace std;

using gyro::vector3;

EllipsoidCalibration::EllipsoidCalibration(QWidget* parent) : QWidget(parent) {

    setupUi(this);
    currentLabel->setFont(QFont("Courier"));
    diffLabel->setFont(QFont("Courier"));

    setVisible(true);
    showMaximized();

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void EllipsoidCalibration::onNewSampleReceived(const AccelMagSample sample) {

    previous = current;

    current = sample;

    currentLabel->setText("Current: "+current.timeStr()+";  "+current.accelStr()+"; "+current.magStr());

    vector3 accelDiff = current.acceleration() - previous.acceleration();

    vector3 magnDiff = current.magnetometerReading() - previous.magnetometerReading();

    diffLabel->setText("Change:  " + vec2QStr(accelDiff) + "; " + vec2QStr(magnDiff) );
}

void EllipsoidCalibration::addItem(int col, const QString& str) {

    // FIXME Who the hell deletes it?
    QTableWidgetItem* item = new QTableWidgetItem(str);

    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    tableWidget->setItem(0, col, item);
}

void EllipsoidCalibration::on_captureButton_pressed() {

    if (current.isNull()) {
        QMessageBox::warning(this, "Warning", "Connect to a mote first!");
        return;
    }

    enum { ACCEL, MAGN, TEMP };

    tableWidget->insertRow(0);

    addItem(ACCEL, current.accelStr());
    addItem(MAGN,  current.magStr());
    addItem(TEMP,  current.tempStr());
}

void EllipsoidCalibration::on_saveButton_clicked() {

    if (tableWidget->rowCount()==0) {

        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));

    // TODO Write to file
}

void EllipsoidCalibration::on_clearButton_clicked() {

    if (tableWidget->rowCount()==0) {

        return;
    }

    // FIXME How to make Cancel the default button?
    const int ret = QMessageBox::warning(this, "Warning", "Dropping all data!\nAre you sure?",
                                         QMessageBox::Yes, QMessageBox::Cancel);

    if (ret==QMessageBox::Yes) {

        clearAll();
    }
}

void EllipsoidCalibration::clearAll() {

    const int size = tableWidget->rowCount();

    for (int i=size-1; i>=0; --i) {

        tableWidget->removeRow(i);
    }

    current = previous = AccelMagSample();

    currentLabel->clear();

    diffLabel->clear();
}

void EllipsoidCalibration::on_tableWidget_cellDoubleClicked(int row, int column) {

    deleteRow(row);

}

void EllipsoidCalibration::on_tableWidget_itemActivated(QTableWidgetItem* item) {

    deleteRow(item->row());
}

void EllipsoidCalibration::deleteRow(int row) {

    const int ret = QMessageBox::warning(this, "Warning", "Do you want to drop this sample?",
                                         QMessageBox::Yes, QMessageBox::Cancel);

    if (ret==QMessageBox::Yes) {

        tableWidget->removeRow(row);
    }
}
