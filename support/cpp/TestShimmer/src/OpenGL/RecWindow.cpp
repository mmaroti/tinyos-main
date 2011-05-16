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

#include <algorithm>
#include <iostream>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFile>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include "RecWindow.hpp"
#include "ArmAngles.hpp"
#include "ArmWidget.hpp"
#include "Globals.hpp"

namespace {

    const qint64 INVALID_REC_ID = -1;
}

RecWindow* RecWindow::right() {

    return new RecWindow(ArmWidget::right(), ArmAngles::right());
}

RecWindow* RecWindow::left() {

    return new RecWindow(ArmWidget::left(), ArmAngles::left());
}

RecWindow::RecWindow(ArmWidget* w, const ArmAngles& c)
    : widget(w), calculator(c), recID(INVALID_REC_ID)
{

    widget->setParent(this);

    widget->show();

    init();

    //setWindowModality(Qt::ApplicationModal);

    setCapturingState();
}

void RecWindow::init() {

    createButtons();

    setupLayout();

    setupConnections();
}

void RecWindow::createButtons() {

    setReferenceButton = new QPushButton("Set reference", this);
    captureButton      = new QPushButton("Capture", this);
    finishedButton     = new QPushButton("Finished", this);
    nextFrameButton    = new QPushButton("Next frame", this);
    dropFrameButton    = new QPushButton("Drop frame", this);
    saveButton         = new QPushButton("Save", this);
    clearButton        = new QPushButton("Clear", this);
}

void RecWindow::setCapturingState() {

    globals::connect_Ellipsoid_AccelMagMsgReceiver();

    frames.clear();
    origSamp.clear();

    frames.reserve(10);
    origSamp.reserve(10);

    matrices.clear();
    samples.clear();

    recID = INVALID_REC_ID;
    setTitle();
    saved = false;

    setReferenceButton->setEnabled(true);
    captureButton->setEnabled(true);
    finishedButton->setEnabled(true);

    nextFrameButton->setDisabled(true);
    dropFrameButton->setDisabled(true);
    saveButton->setDisabled(true);

    clearButton->setEnabled(true);

    setReferenceButton->setFocus();
}

void RecWindow::setEditingState() {

    globals::disconnect_Ellipsoid_AccelMagMsgReceiver();

    setReferenceButton->setDisabled(true);
    captureButton->setDisabled(true);
    finishedButton->setDisabled(true);

    nextFrameButton->setEnabled(true);
    dropFrameButton->setEnabled(true);
    saveButton->setEnabled(true);    
    clearButton->setEnabled(true);

    frameIndex = 0;
    displayCurrentFrame();

    setTitle();
    saved = false;

    nextFrameButton->setFocus();
}

void RecWindow::setTitle() {

    QString recIDStr = (recID==INVALID_REC_ID) ? QString() : (" #"+QString::number(recID));

    QString birthDay = person.birth().toString(Qt::ISODate);

    setWindowTitle(person.name()+"  "+birthDay+recIDStr);
}

void RecWindow::setupLayout() {

    QHBoxLayout* controls = new QHBoxLayout();

    controls->addWidget(setReferenceButton);
    controls->addWidget(captureButton);
    controls->addWidget(finishedButton);
    controls->addWidget(nextFrameButton);
    controls->addWidget(dropFrameButton);
    controls->addWidget(saveButton);
    controls->addWidget(clearButton);

    QGridLayout* mainLayout = new QGridLayout(this);

    mainLayout->addWidget(widget, 0, 0);
    mainLayout->addLayout(controls, 1, 0);

    setLayout(mainLayout);
}

void RecWindow::setupConnections() {

    connect(setReferenceButton, SIGNAL(clicked()), SLOT(setReferenceClicked()));
    connect(captureButton,      SIGNAL(clicked()), SLOT(captureClicked()));
    connect(finishedButton,     SIGNAL(clicked()), SLOT(finishedClicked()));
    connect(nextFrameButton,    SIGNAL(clicked()), SLOT(nextFrameClicked()));
    connect(dropFrameButton,    SIGNAL(clicked()), SLOT(dropFrameClicked()));
    connect(saveButton,         SIGNAL(clicked()), SLOT(saveClicked()));
    connect(clearButton,        SIGNAL(clicked()), SLOT(clearClicked()));
}

// TODO Samples should calibrate themselves and save raw samples!
void RecWindow::updateMatrix(const AccelMagSample sample) {

    if (!sample.isStatic()) {

        return;
    }

    const int mote = sample.moteID();

    const matrix3 rotMat = sample.toRotationMatrix();

    matrices[mote] = rotMat;
    samples[mote]  = sample;

    display();
}

void RecWindow::displayCurrentFrame() {

    if (frames.empty()) {

        return;
    }

    frameIndex %= frames.size();

    matrices = frames.at(frameIndex);

    display();
}

void RecWindow::display() {

    std::vector<std::string> text = calculator.labels(matrices, frameIndex, frames.size());

    std::vector<std::string> table = calculator.table(frames);

    text.insert(text.end(), table.begin(), table.end());

    widget->display(matrices, text);
}

void RecWindow::setReferenceClicked() {

    headings = calculator.setHeading(matrices);

    widget->setReference(headings);

    captureButton->setFocus();
}

void RecWindow::captureClicked() {

    // Check matrices.size()
    frames.push_back(matrices);
    origSamp.push_back(samples);

    setReferenceButton->setDisabled(true);
}

void RecWindow::finishedClicked() {

    setEditingState();
}

void RecWindow::nextFrameClicked() {

    if (!frames.empty()) {

        ++frameIndex;

        displayCurrentFrame();
    }
}

void RecWindow::dropFrameClicked() {

    if (frames.empty() || !areYouSure("Dropping frame.")) {

        return;
    }

    saved = false;

    typedef std::vector<MatMap>::iterator itr;

    itr pos = frames.begin()+frameIndex;

    Q_ASSERT(pos<frames.end());

    frames.erase(pos);
    origSamp.erase(origSamp.begin()+frameIndex);

    if (!frames.empty()) {

        displayCurrentFrame();

        nextFrameButton->setFocus();
    }
    else {

        setCapturingState();
    }
}

void RecWindow::saveClicked() {

    if (!frames.empty()) {

        // TODO Push to SQLite database
        writeRecord();
    }
    else {
        // Warning? Nothing to save?
    }

    saved = true;
}

void RecWindow::clearClicked() {

    if (frames.empty() || saved || areYouSure("Dropping all frames.")) {

        setCapturingState();
    }
}

bool RecWindow::areYouSure(const char* text) {

    const int ret = QMessageBox::warning(this, "Warning", text+QString("\nAre you sure?"),
                                         QMessageBox::Yes, QMessageBox::Cancel);

    return ret==QMessageBox::Yes;
}

void RecWindow::writeRecord() const {

    const QString REC_DIR = "../rec/";

    QString fileName = "s"+QString::number(recID)+".csv";

    QFile file(REC_DIR+fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream out(&file);

        writeData(out);
    }
    else {

        Q_ASSERT(false);
    }
}

template <typename Value>
class IntKeyWriter {

    QTextStream& out;

public:

    IntKeyWriter(QTextStream& out) : out(out) { }

    void operator()(const std::pair<int,Value>& p) { out << p.first << '\n'; }
};

template <typename T>
class STLVecWriter {

    QTextStream& out;

public:

    STLVecWriter(QTextStream& out) : out(out) { }

    void operator()(const T& elem) { out << elem << '\n'; }
};

template <typename T>
const QString toCSV(const T& mv, const int length) {

    double r[length];

    mv.copy_to(r);

    QString result;

    for (int i=0; i<length; ++i) {

        result += (QString::number(r[i],'e',16)+';');
    }

    result.chop(1);

    return result;
}

const QString toCSV(const gyro::matrix3& m) {

    return toCSV(m, 9);
}

const QString toCSV(const gyro::vector3& v) {

    return toCSV(v, 3);
}

const QString toCSV(const AccelMagSample& s) {

    return toCSV(s.acceleration())+';'+toCSV(s.magnetometerReading());
}

template <typename Key, typename Value>
class STLMapWriter {

    QTextStream& out;

public:

    STLMapWriter(QTextStream& out) : out(out) { }

    void operator()(const std::pair<Key,Value>& p) {
        out << p.first << ';' << toCSV(p.second) << '\n';
    }
};

template <typename Key, typename Value>
class MapVecWriter {

    QTextStream& out;

public:

    MapVecWriter(QTextStream& out) : out(out) { }

    void operator()(const std::map<Key,Value>& map) {

        std::for_each(map.begin(), map.end(), STLMapWriter<Key,Value>(out));
    }
};

void RecWindow::writeData(QTextStream& out) const {

    Q_ASSERT(!frames.empty() && (frames.size()==origSamp.size()));

    out << "# Motes\n";

    const MatMap& m = *frames.begin();

    std::for_each(m.begin(), m.end(), IntKeyWriter<gyro::matrix3>(out));

    out << "# Headings\n";

    std::for_each(headings.begin(), headings.end(), STLVecWriter<double>(out));

    out << "# Frames\n";

    std::for_each(frames.begin(), frames.end(), MapVecWriter<int,gyro::matrix3>(out));

    out << "# Samples\n";

    std::for_each(origSamp.begin(), origSamp.end(), MapVecWriter<int,AccelMagSample>(out));

    out << '\n' << flush;
}
