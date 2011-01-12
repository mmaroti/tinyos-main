/** Copyright (c) 2010, University of Szeged
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
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include <QtGui>

#include "GLWidget.h"
#include "window.h"
#include "Application.h"

Window::Window(Application &app) : application(app)
{
    QGridLayout *mainLayout = new QGridLayout;

    xSlider = createSlider();

    for (int i = 0; i < NumRows; ++i) {
        for (int j = 0; j < NumColumns; ++j) {
            QColor clearColor;
            clearColor.setHsv(0, 255, 63);

            glWidgets[i][j] = new GLWidget(0, 0);
            glWidgets[i][j]->setClearColor(clearColor);
            mainLayout->addWidget(glWidgets[i][j], i, j);

            connect(glWidgets[i][j], SIGNAL(clicked()),
                    this, SLOT(setCurrentGlWidget()));
        }
    }
    mainLayout->addWidget(xSlider);
    setLayout(mainLayout);

    currentGlWidget = glWidgets[0][0];

    timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(rotateToNext()));
    connect(xSlider, SIGNAL(valueChanged(int)), this, SLOT(onSlide(int)) );

    //timer->start(200);
    xSlider->setValue(10);

    //setWindowTitle(tr("Shimmer"));
}

void Window::setCurrentGlWidget()
{
    currentGlWidget = qobject_cast<GLWidget *>(sender());
}

void Window::onSlide(int sample_index)
{
    rotateToNext(sample_index);
}

void Window::rotateToNext(int sample_index)
{

    const DataRecorder& dr = application.dataRecorder;

    if (dr.empty()) {

        return;
    }

    const double* const matrix = dr.at(sample_index).rotmat;

    currentGlWidget->rotate(matrix);
}

void Window::rotateOneStep()
{
    timerCounter++;
    //if (currentGlWidget) currentGlWidget->rotateBy(+2 * 16, +2 * 16, -1 * 16);
    xSlider->setValue(timerCounter);
}

void Window::setTimer(int time)
{
    connect(timer, SIGNAL(timeout()), this, SLOT(rotateOneStep()));
    timer->start(time);
    timerCounter = 0;
}

void Window::stopTimer()
{
    disconnect(timer, SIGNAL(timeout()), this, SLOT(rotateOneStep()));
    timer->stop();
    timerCounter = 0;
}


void Window::load3D()
{
    xSlider->setRange(10, application.dataRecorder.size()-1);
    xSlider->setValue(10);
    xSlider->update();
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(1,application.dataRecorder.size()-1 );
    slider->setSingleStep(1);
    slider->setPageStep(204);
    slider->setTickInterval(204);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

