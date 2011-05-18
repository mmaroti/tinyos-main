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
}

void ArmTab::init() {

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
