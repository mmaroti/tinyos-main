/* Copyright (c) 2011 University of Szeged
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
* Author: Ali Baharev
*/

#include <iostream>
#include <QMessageBox>
#include "ArmTab.hpp"
#include "Globals.hpp"
#include "RecordHandler.hpp"
#include "SQLDialog.hpp"
#include "RecWindow.hpp"

namespace {

    const qint64 INVALID_RECORD_ID = -1;

    const char DATE_FORMAT[] = "yyyy-MM-dd";
}

ArmTab::ArmTab(QWidget *parent)

: QWidget(parent),
recordSelector(globals::recordSelector()),
personSelector(globals::personSelector())

{

    setupUi(this);

    QFont defaultBoldFont = QFont();

    defaultBoldFont.setBold(true);

    Person_Label->setFont(defaultBoldFont);

    init();

    connect(recordSelector, SIGNAL(recordSelected(qint64,Person,MotionType)),
                            SLOT(onRecordSelected(qint64,Person,MotionType)));

    connect(personSelector, SIGNAL(personSelected(Person)),
                            SLOT(onPersonSelected(Person)));

    warningIsShown = false;
}

void ArmTab::init() {

    disconnectCalibration();

    person = Person();

    recordID = INVALID_RECORD_ID;

    Person_Label->clear();

    Open_Existing->setEnabled(true);

    Select_Person->setEnabled(true);

    Select_Person->setFocus();

    Motion_Type->setCurrentIndex(0);

    Motion_Type->setDisabled(true);

    Start->setDisabled(true);

    New_With_Person->setDisabled(true);

    New_Record->setEnabled(true);
}

void ArmTab::setRecordingState() {

    Open_Existing->setDisabled(true);

    Select_Person->setDisabled(true);

    Motion_Type->setDisabled(true);

    Start->setDisabled(true);

    New_With_Person->setEnabled(true);

    New_Record->setEnabled(true);

    New_With_Person->setFocus();
}

void ArmTab::on_Open_Existing_clicked() {

    recordSelector->show();

    recordSelector->activateWindow();
}

void ArmTab::onRecordSelected(qint64 recID, const Person& p, MotionType type) {

    disconnectCalibration();

    recordID = recID;

    person = p;

    if (type!=RIGHT_ELBOW_FLEX && type!=LEFT_ELBOW_FLEX) {

        applicationCrash("invalid motion type: "+QString::number(type));
    }

    Motion_Type->setCurrentIndex(type);

    setPersonLabel();

    setRecordingState();

    checkConsistency();

    RecWindow* rw = getRecWindow();

    rw->showMaximized();
}

RecWindow* ArmTab::getRecWindow() {

    RecWindow* rw = RecWindow::createRecWindow(recordID, person, getMotionType());

    connect(rw, SIGNAL(saveAngles(QString)), SLOT(saveAngles(QString)));

    connect(this, SIGNAL(anglesSaved(qint64)), rw, SLOT(anglesSaved(qint64)));

    return rw;
}

void ArmTab::on_Select_Person_clicked() {

    personSelector->show();

    personSelector->activateWindow();
}

void ArmTab::onPersonSelected(const Person& p) {

    disconnectCalibration();

    person = p;

    recordID = INVALID_RECORD_ID;

    checkPersonValidity();

    setPersonLabel();

    Motion_Type->setCurrentIndex(0);

    Motion_Type->setEnabled(true);

    Motion_Type->setFocus();
}

void ArmTab::setPersonLabel() {

    Person_Label->setText(person.name()+"   "+person.birth().toString(DATE_FORMAT));
}

void ArmTab::on_Motion_Type_currentIndexChanged(int index) {

    if (index==0) {

        Start->setDisabled(true);
    }
    else {

        Start->setEnabled(true);

        Start->setFocus();
    }
}

void ArmTab::on_Start_clicked()
{

    disconnectCalibration();

    checkConsistency();

    if (recordID != INVALID_RECORD_ID) {

        applicationCrash("record ID not cleared");
    }

    setRecordingState();

    RecWindow* rw = getRecWindow();

    rw->showMaximized();
}

void ArmTab::saveAngles(QString table) {

    checkPersonValidity();

    if (recordID==INVALID_RECORD_ID) {

        recordID = recordSelector->insertRecord(person.id(), getMotionType(), table);
    }
    else {

        recordSelector->updateRecord(recordID, table);
    }

    emit anglesSaved(recordID);
}

void ArmTab::on_New_With_Person_clicked() {

    Person p = person;

    init();

    onPersonSelected(p);
}

void ArmTab::on_New_Record_clicked() {

    init();
}

void ArmTab::applicationCrash(const QString& msg) const {

    QMessageBox::critical(0, "Application crash", "Exiting, fatal error: "+msg);

    exit(EXIT_FAILURE);
}

void ArmTab::checkPersonValidity() const {

    if (person.isNull()) {

        applicationCrash("person is NULL");;
    }
}

void ArmTab::checkConsistency() const {

    QString errorMsg;

    if (person.isNull()) {

        errorMsg = "person is not set; ";
    }

    if (!nameIsConsistent()) {

        errorMsg += "person label not updated; ";
    }

    const QDate birth = person.birth();

    if (!birth.isValid()) {

        errorMsg += "invalid date of birth; ";
    }

    if (Motion_Type->currentIndex() == 0) {

        errorMsg += "motion type not set; ";
    }

    if (!errorMsg.isEmpty()) {

        applicationCrash(errorMsg);
    }
}

bool ArmTab::nameIsConsistent() const {

    return Person_Label->text().startsWith(person.name(), Qt::CaseInsensitive);
}

MotionType ArmTab::getMotionType() const {

    const QString motionType = Motion_Type->currentText();

    if (motionType == "Right elbow flex") {

        return RIGHT_ELBOW_FLEX;
    }
    else if (motionType == "Left elbow flex") {

        return LEFT_ELBOW_FLEX;
    }
    else {

        applicationCrash("motion type should have been set");

        return RIGHT_ELBOW_FLEX;
    }
}

//======================================================================
// FIXME These should go to a separate class
// TODO Clean up code after deadline!!!

void ArmTab::on_Connect_Button_clicked() {

    QString text = Connect_Button->text();

    if (text=="Connect") {

        bool success = globals::connect_ArmTab_AccelMagMsgReceiver(this);

        if (success) {

            Connect_Button->setText("Disconnect");
        }
    }
    else {

        disconnectCalibration();
    }
}

void ArmTab::onNewSample(AccelMagSample s) {

    if (warningIsShown) {

        return;
    }

    if (!sample.isNull() && sample.moteID()!=s.moteID()) {

        warningIsShown = true;

        QMessageBox::warning(this, "Warning", "Either another mote is on or the table is not cleared!");

        warningIsShown = false;

        return;
    }

    sample = s;

    SampleLabel->setText("Mote "+sample.moteStr()+":  "+sample.accelStr()+";  "+sample.magStr()+";  "+sample.tempStr());
}

void ArmTab::on_Capture_Button_clicked() {

    if (sample.isNull()) {

        return;
    }

    for (int i=0; i<3; ++i) {

        bool updated = checkCoordinate(i);

        if (updated) {

            break;
        }
    }
}

bool ArmTab::checkCoordinate(const int i) {

    double acc = sample.acceleration(i);

    if (acc > 0.7) {

        updatePositive(i);

        return true;
    }
    else if (acc < -0.7) {

        updateNegative(i);

        return true;
    }

    return false;
}

void ArmTab::updatePositive(const int i) {

    QTableWidgetItem* acc_item = new QTableWidgetItem;
    acc_item->setData(Qt::DisplayRole, sample.acceleration(i));

    QTableWidgetItem* magn_item = new QTableWidgetItem;
    magn_item->setData(Qt::DisplayRole, sample.magnetometer(i));

    tableWidget->setItem(i+1, 1, acc_item);
    tableWidget->setItem(i+1, 4, magn_item);
}

void ArmTab::updateNegative(const int i) {

    QTableWidgetItem* acc_item = new QTableWidgetItem;
    acc_item->setData(Qt::DisplayRole, sample.acceleration(i));

    QTableWidgetItem* magn_item = new QTableWidgetItem;
    magn_item->setData(Qt::DisplayRole, sample.magnetometer(i));

    tableWidget->setItem(i+1, 2, acc_item);
    tableWidget->setItem(i+1, 3, magn_item);
}

void ArmTab::on_Clear_Button_clicked() {

    if (areYouSure("Dropping all data.")) {

        clearCalibrationContents();

        sample = AccelMagSample();
    }
}

void ArmTab::clearCalibrationContents() {

    for (int i=1; i<=3; ++i) {

        for (int j=1; j<=4; ++j) {

            tableWidget->setItem(i, j, new QTableWidgetItem);

        }
    }

    SampleLabel->clear();
}

void ArmTab::on_Done_Button_clicked() {

    std::vector<std::vector<double> > values;

    values.resize(3);

    for (int i=0; i<3; ++i) {

        values.at(i).resize(4);
    }

    bool isOK = fillWithTableValues(values);

    if (!isOK) {

        QMessageBox::critical(this, "Error", "Missing or corrupt values!");

        return;
    }

    computeScalesOffSets(values);

}

void ArmTab::computeScalesOffSets(std::vector<std::vector<double> >& values) {

    enum { ACC_POS, ACC_NEG, MAGN_POS, MAGN_NEG };

    double acc_scale[3];
    double acc_offset[3];
    double magn_scale[3];
    double magn_offset[3];

    for (int i=0; i<3; ++i) {

        double pos = values.at(i).at(ACC_POS);
        double neg = values.at(i).at(ACC_NEG);

        acc_scale[i]  = 2.0/(pos-neg);
        acc_offset[i] = (pos+neg)/2.0;
    }

    for (int i=0; i<3; ++i) {

        double pos = values.at(i).at(MAGN_POS);
        double neg = values.at(i).at(MAGN_NEG);

        magn_scale[i]  = 2.0/(pos-neg);
        magn_offset[i] = (pos+neg)/2.0;
    }

    using gyro::vector3;
    std::cout << "Acc scale:   " << vector3(acc_scale) << std::endl;
    std::cout << "Acc offset:  " << vector3(acc_offset) << std::endl;
    std::cout << "Magn scale:  " << vector3(magn_scale) << std::endl;
    std::cout << "Magn offset: " << vector3(magn_offset) << std::endl;

    bool success = globals::updateScaleOffset(sample.moteID(), vector3(acc_scale),  vector3(acc_offset),
                                                               vector3(magn_scale), vector3(magn_offset));

    if (success) {

        QMessageBox::information(this, "", "Calibration data of mote "+sample.moteStr()+" successfully updated!");
    }
}

void ArmTab::on_Reset_Button_clicked() {

    if (sample.isNull()) {

        return;
    }

    if (areYouSure("This will drop calibration data of mote "+QString::number(sample.moteID()).toAscii())) {

        globals::resetScaleOffset(sample.moteID());

        clearCalibrationContents();
    }
}

bool ArmTab::fillWithTableValues(std::vector<std::vector<double> >& values) {

    for (int i=1; i<=3; ++i) {

        for (int j=1; j<=4; ++j) {

            QTableWidgetItem* item = tableWidget->item(i, j);

            if (item==0) {

                return false;
            }

            QVariant data = item->data(Qt::DisplayRole);

            bool isOK = false;

            values.at(i-1).at(j-1) = data.toDouble(&isOK);

            if (!isOK) {

                return false;
            }

        }
    }

    return true;
}

bool ArmTab::areYouSure(const char* text) {

    const int ret = QMessageBox::warning(this, "Warning", text+QString("\nAre you sure?"),
                                         QMessageBox::Yes, QMessageBox::Cancel);

    return ret==QMessageBox::Yes;
}

void ArmTab::disconnectCalibration() {

    globals::disconnect_ArmTab_AccelMagMsgReceiver(this);

    Connect_Button->setText("Connect");
}
