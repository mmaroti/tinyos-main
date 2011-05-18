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

#ifndef ARMTAB_HPP
#define ARMTAB_HPP

#include "ui_Arm.h"
#include "AccelMagSample.hpp"
#include "MotionTypes.hpp"
#include "Person.hpp"

class RecWindow;
class RecordHandler;
class SQLDialog;

class ArmTab : public QWidget, private Ui::Arm {

    Q_OBJECT

public:

    ArmTab(QWidget* parent);

public slots:

    void saveAngles(QString table);

    void onNewSample(AccelMagSample sample);

signals:

    void anglesSaved(qint64 recordID);

private slots:

    void on_Reset_Button_clicked();
    void on_Done_Button_clicked();
    void on_Clear_Button_clicked();
    void on_Capture_Button_clicked();
    void on_Connect_Button_clicked();
    void on_New_Record_clicked();
    void on_New_With_Person_clicked();
    void on_Start_clicked();
    void on_Motion_Type_currentIndexChanged(int index);
    void on_Select_Person_clicked();
    void on_Open_Existing_clicked();

    void onPersonSelected(const Person& p);
    void onRecordSelected(qint64 recID, const Person& p, MotionType type);

private:

    void init();

    void setRecordingState();

    RecWindow* getRecWindow();

    void setPersonLabel();

    void checkPersonValidity() const;

    void checkConsistency() const;

    bool nameIsConsistent() const;

    void applicationCrash(const QString& msg) const;

    bool checkCoordinate(const int coordinate);

    void updatePositive(const int coordinate);

    void updateNegative(const int coordinate);

    bool fillWithTableValues(std::vector<std::vector<double> >& values);

    void computeScalesOffSets(std::vector<std::vector<double> >& values);

    bool areYouSure(const char* text);

    MotionType getMotionType() const;

    RecordHandler* const recordSelector;
    SQLDialog* const personSelector;

    qint64 recordID;
    Person person;

    AccelMagSample sample;
    bool warningIsShown;
};

#endif // ARMTAB_HPP
