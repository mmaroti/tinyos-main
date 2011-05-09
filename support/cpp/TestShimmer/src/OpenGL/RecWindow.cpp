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
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include "RecWindow.hpp"
#include "ArmWidget.hpp"

RecWindow* RecWindow::right() {

    return new RecWindow(ArmWidget::right());
}

RecWindow* RecWindow::left() {

    return new RecWindow(ArmWidget::left());
}

RecWindow::RecWindow(ArmWidget* w) : widget(w) {

    widget->setParent(this);

    widget->show();

    init();

    setWindowModality(Qt::ApplicationModal);

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

    // TODO Connect updateMatrix
    frames.clear();
    frames.reserve(10);

    setReferenceButton->setEnabled(true);
    captureButton->setEnabled(true);
    finishedButton->setEnabled(true);

    nextFrameButton->setDisabled(true);
    dropFrameButton->setDisabled(true);
    saveButton->setDisabled(true);
    clearButton->setDisabled(true);
}

void RecWindow::setEditingState() {

    // TODO Disconnect updateMatrix
    frameIndex = 0;

    setReferenceButton->setDisabled(true);
    captureButton->setDisabled(true);
    finishedButton->setDisabled(true);

    nextFrameButton->setEnabled(true);
    dropFrameButton->setEnabled(true);
    saveButton->setEnabled(true);
    clearButton->setEnabled(true);

    // TODO Set frames.at(0) to widget here!
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
    connect(clearButton,        SIGNAL(clicked()), SLOT(clearClicked()));
}

void RecWindow::keyPressEvent(QKeyEvent * event) {

    if (event->key() == Qt::Key_Space) {

    }
}

void RecWindow::updateMatrix(int mote, const gyro::matrix3 rotMat) {

    // TODO Move all calibration-related stuff here and save raw samples as well
    matrices[mote] = rotMat;

    // TODO Perhaps the map should be passed?
    widget->setFrame(mote, rotMat);
}

void RecWindow::displayCurrentFrame() {

    frameIndex %= frames.size();

    matrices = frames.at(frameIndex);

    // TODO widget->display(matrices);
}

void RecWindow::setReferenceClicked() {

    typedef std::map<int,gyro::matrix3>::const_iterator itr;

    for (itr i=matrices.begin(); i!=matrices.end(); ++i) {

        widget->setReference(i->first, i->second);
    }
}

void RecWindow::captureClicked() {

    frames.push_back(matrices);
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

    if (frames.empty()) {

        return;
    }

    typedef std::vector<Map>::iterator itr;

    itr pos = frames.begin()+frameIndex;

    Q_ASSERT(pos<frames.end());

    frames.erase(pos);

    displayCurrentFrame();
}

void RecWindow::saveClicked() {

}

void RecWindow::clearClicked() {

    // TODO Clear all other state here or in setCapturingState()
    setCapturingState();
}
