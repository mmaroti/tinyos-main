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

#include <cmath>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include "EllipsoidCalibration.hpp"
#include "RecWindow.hpp"

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

    recWindow = 0;
}

void EllipsoidCalibration::onNewSampleReceived(const AccelMagSample sample) {

    // FIXME Check mote ID!
    previous = current;

    current = sample;

    currentLabel->setText("Current: "+current.timeStr()+";  "+current.accelStr()+"; "+current.magStr());

    vector3 accelDiff = current.acceleration() - previous.acceleration();

    vector3 magnDiff = current.magnetometerReading() - previous.magnetometerReading();

    diffLabel->setText("Change:  " + vec2QStr(accelDiff) + "; " + vec2QStr(magnDiff) );

    if (recWindow==0) {

        recWindow = RecWindow::right();

        recWindow->showMaximized();
    }
    else {

        recWindow->updateMatrix(sample);
    }
}

void EllipsoidCalibration::addItem(Column col, const QString& str) {

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

    tableWidget->insertRow(0);

    addItem(ACCEL, current.accelStr());
    addItem(MAGN,  current.magStr());
    addItem(TEMP,  current.tempStr());
}

void EllipsoidCalibration::on_saveButton_clicked() {

    if (tableWidget->rowCount()==0) {

        return;
    }

    QString name = QFileDialog::getSaveFileName(this, tr("Save File"));

    if (name.isEmpty()) {

        return;
    }

    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QMessageBox::critical(this, "Error", "Failed to create file!");
        return;
    }

    QTextStream out(&file);

    exportSamples(out);

    file.close();
}

void EllipsoidCalibration::exportSamples(QTextStream &out) {

    out << "# Mote ID\n";
    out << current.moteID() << '\n';
    out << "# Samples: accel(x,y,z), magnetometer(x,y,z), temperature\n";

    const int size = tableWidget->rowCount();

    for (int i=0; i<size; ++i) {

        const QTableWidgetItem* acc  = tableWidget->item(i, ACCEL);
        const QTableWidgetItem* magn = tableWidget->item(i, MAGN);
        const QTableWidgetItem* temp = tableWidget->item(i, TEMP);

        out << acc->text() << ", " << magn->text() << ", " << temp->text() << '\n';
    }

    out << '\n';

    out.flush();
}

void EllipsoidCalibration::on_clearButton_clicked() {

    if (tableWidget->rowCount()==0) {

        clearSamples();

        return;
    }

    // FIXME How to make Cancel the default button?
    const int ret = QMessageBox::warning(this, "Warning", "Dropping all data!\nAre you sure?",
                                         QMessageBox::Yes, QMessageBox::Cancel);

    if (ret==QMessageBox::Yes) {

        clearAll();

        recWindow->showMaximized();
    }
}

void EllipsoidCalibration::clearSamples() {

    current = previous = AccelMagSample();

    currentLabel->clear();

    diffLabel->clear();
}

void EllipsoidCalibration::clearRows() {

    const int size = tableWidget->rowCount();

    for (int i=size-1; i>=0; --i) {

        tableWidget->removeRow(i);
    }
}

void EllipsoidCalibration::clearAll() {

    clearRows();

    clearSamples();
}

void EllipsoidCalibration::on_tableWidget_cellDoubleClicked(int row, int ) {

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
