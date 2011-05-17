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

#ifndef RECWINDOW_HPP
#define RECWINDOW_HPP

#include <map>
#include <vector>
#include <QWidget>
#include "AccelMagSample.hpp"
#include "ArmAngles.hpp"
#include "MatrixVector.hpp"
#include "Person.hpp"
#include "MotionTypes.hpp"

class QPushButton;
class QTextStream;
class ArmWidget;

class RecWindow : public QWidget
{
    Q_OBJECT

public:

    static RecWindow* createRecWindow(const qint64 recordID, const Person& p, const MotionType type);

    // TODO Should reference heading and matrices be cleared on show/close?
    // TODO Enable/disable buttons on close?

public slots:

    void updateMatrix(const AccelMagSample sample);

private slots:

    void setReferenceClicked();
    void captureClicked();
    void finishedClicked();
    void nextFrameClicked();
    void dropFrameClicked();
    void saveClicked();
    void clearClicked();

private:

    Q_DISABLE_COPY(RecWindow)

    RecWindow(ArmWidget* w, const ArmAngles& c);

    void init();
    void createButtons();
    void setupLayout();
    void setupConnections();

    void clearContainers();
    void setCapturingState();
    void setEditingState();
    void setTitle();

    void displayCurrentFrame();
    void display();

    bool areYouSure(const char* text);

    // TODO Move to its own class
    void writeRecord() const;
    void writeData(QTextStream& out) const;
    void readRecord();

    void readData(QTextStream& in);
    void readFrameLine(const QString& buffer);
    void readSampleLine(const QString& buffer);
    const QString filename() const;

    QPushButton* setReferenceButton;
    QPushButton* captureButton;
    QPushButton* finishedButton;

    QPushButton* nextFrameButton;
    QPushButton* dropFrameButton;
    QPushButton* saveButton;
    QPushButton* clearButton;

    ArmWidget* widget;
    ArmAngles calculator;

    qint64 recID;
    Person person;

    // TODO All these containers should be moved to their own class
    typedef std::map<int,gyro::matrix3> MatMap;
    typedef std::map<int,AccelMagSample> SampMap;

    MatMap  matrices;
    SampMap samples;

    std::vector<MatMap>  frames;
    std::vector<SampMap> origSamp;
    std::vector<double> headings;

    size_t frameIndex;

    bool saved;
};

#endif // RECWINDOW_HPP
