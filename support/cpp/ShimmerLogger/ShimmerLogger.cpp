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
* - Neither the name of the University of Szeged nor the names of its
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
*/

/**
* @author Andras Becsi (abecsi@inf.u-szeged.hu)
*/

#include "ShimmerLogger.h"
#include <QtGui>
#include <QVBoxLayout>
#include "DeviceManager.h"

ShimmerLogger::ShimmerLogger()
{
    setupUI();
}

void ShimmerLogger::setupUI()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //MenuBar
    QAction* quit = new QAction("Kilépés", this);
    quit->setToolTip("Kilépés a programból.");
    QAction* importFromFile = new QAction("Megnyitás", this);
    importFromFile->setToolTip("Adatfájl importálása.");

    QAction* reset = new QAction("Tisztítás", this);

    QMenu* fileMenu = new QMenu("Fájl", this);
    QMenu* toolsMenu = new QMenu("Eszközök", this);

    connect(quit, SIGNAL(triggered()), SLOT(close()));
    connect(importFromFile, SIGNAL(triggered()), SLOT(importActionClicked()));
    connect(reset, SIGNAL(triggered()), SLOT(resetPlot()));

    fileMenu->addAction(importFromFile);
    fileMenu->addSeparator();
    fileMenu->addAction(quit);
    toolsMenu->addAction(reset);
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(toolsMenu);

    QToolBar* toolBar = new QToolBar(this);
    toolBar->setFixedHeight(50);
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    //clock = new ClockWidget(this);

    controlButton = new QPushButton("Felvétel", this);
    connect(controlButton,SIGNAL(clicked()),this, SLOT(handleControlButton()));
    toolBar->addWidget(controlButton);
    toolBar->addSeparator();
    //toolBar->addWidget(clock);
    addToolBar(toolBar);
    QToolBar* sliderBar = new QToolBar(this);

    //CentralWidget
    plotter = new PlotWidget(this);
    //plotter->setClock(clock);
    manager = new DeviceManager(this);
    manager->setPlotter(plotter);
    slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::TopScale);
    sliderBar->addWidget(slider);
    addToolBar(Qt::BottomToolBarArea, sliderBar);
    slider->setThumbLength(50);
    slider->setBorderWidth(1);
    slider->setScalePosition(QwtSlider::NoScale);
    slider->setBgStyle(QwtSlider::BgBoth);
    updateSlider();

    connect(slider, SIGNAL(sliderPressed()), plotter, SLOT(pause()));
    connect(slider, SIGNAL(sliderMoved(double)), plotter, SLOT(sliderValueChanged(double)));
    connect(slider, SIGNAL(sliderReleased()), plotter, SLOT(setShowMarker()));
    connect(slider, SIGNAL(sliderReleased()), plotter, SLOT(resume()));
    connect(slider, SIGNAL(sliderReleased()), plotter, SLOT(replot()));
    connect(plotter, SIGNAL(sliderUpdateNeeded(double,double)), SLOT(updateSlider(double,double)));
    plotter->setTimerInterval(UPDATE_TIMER);
    setCentralWidget( plotter );
    manager->start();
}

void ShimmerLogger::handleControlButton() {
    if (controlButton->text() == "Felvétel") {
        manager->resume();
        controlButton->setText("Mentés");
        plotter->resume();
    } else {
        manager->stop();
        plotter->pause();
        plotter->selectFile();
        controlButton->setText("Felvétel");
    }
}

void ShimmerLogger::importActionClicked() {
    plotter->selectFile(false);
}

void ShimmerLogger::unimplemented()
{
    QMessageBox::critical(this,"Unimplemented","This slot is not implemented yet!");
}

void ShimmerLogger::updateSlider(double value, double end) {
        if (end)
            slider->setRange(0.0, end, 1);
        slider->setValue(value);
}

void ShimmerLogger::resetPlot() {
    handleControlButton();
    plotter->clearAndInit();
}
