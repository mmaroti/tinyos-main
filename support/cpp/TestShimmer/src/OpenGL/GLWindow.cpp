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

#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include "GLWindow.hpp"
#include "GLRightElbowFlex.hpp"

GLWindow::GLWindow() : ANIMATION_STEP_MS(20) {

    setAttribute(Qt::WA_DeleteOnClose);

    createGLWidget();

    init();
}

GLWindow::GLWindow(double* rotmat, int size) : ANIMATION_STEP_MS(20) {

    createGLWidget(rotmat, size);

    init();
}

void GLWindow::init() {

    createSlider();

    createButton();

    createTimer();

    setupLayout();

    setupConnections();

    timerStart();
}

void GLWindow::createGLWidget() {

    widget = new GLRightElbowFlex(AnimationElbowFlexSign::right(), this, 0);

    widget->setData("MMtricky2");
}


void GLWindow::createGLWidget(double* rotmat, int size) {

    widget = new GLRightElbowFlex(AnimationElbowFlexSign::right(), this, 0); // FIXME Duplication

    widget->setData(rotmat, size);
}

void GLWindow::createSlider() {

    slider = new QSlider(Qt::Horizontal, this);

    slider->setRange(0, widget->numberOfSamples()-1);

    slider->setSingleStep(4);

    slider->setPageStep(205); // FIXME Knows the sampling rate

    slider->setTickInterval(205);

    slider->setTickPosition(QSlider::TicksBothSides);
}

void GLWindow::createButton() {

    playButton = new QPushButton("Pause", this);

    int width = playButton->width();

    playButton->setFixedWidth(width);
}

void GLWindow::createTimer() {

    timer = new QTimer(this);
}

void GLWindow::setupLayout() {

    QHBoxLayout* controls = new QHBoxLayout();

    controls->addWidget(playButton);

    controls->addWidget(slider);

    QGridLayout* mainLayout = new QGridLayout(this);

    mainLayout->addWidget(widget, 0, 0);

    mainLayout->addLayout(controls, 1, 0);

    setLayout(mainLayout);
}

void GLWindow::setupConnections() {

    connect(timer, SIGNAL(timeout()), SLOT(nextFrame()));

    connect(slider, SIGNAL(valueChanged(int)), SLOT(setFrame(int)));

    connect(widget, SIGNAL(clicked()), SLOT(toggleAnimationState()));

    connect(playButton, SIGNAL(pressed()), SLOT(toggleAnimationState()));
}

void GLWindow::nextFrame() {

    slider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

void GLWindow::setFrame(int pos) {

    if (pos == slider->maximum()) {

        timerStop();
    }

    widget->setFrame(pos);
}

void GLWindow::keyPressEvent(QKeyEvent * event) {

    if (event->key() == Qt::Key_Space) {

        toggleAnimationState();
    }
}

void GLWindow::toggleAnimationState() {

    if (timer->isActive()) {

        timerStop();
    }
    else {

        timerStart();
    }
}

void GLWindow::timerStart() {

    playButton->setText("Pause");

    if (slider->sliderPosition() == slider->maximum()) {

        slider->triggerAction(QAbstractSlider::SliderToMinimum);
    }

    timer->start(ANIMATION_STEP_MS);
}

void GLWindow::timerStop() {

    playButton->setText("Play");

    timer->stop();
}

