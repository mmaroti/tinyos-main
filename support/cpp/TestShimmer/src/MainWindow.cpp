/** Copyright (c) 2010, 2011, University of Szeged
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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include <QScrollArea>
#include "Application.h"
#include "CalibrationWidget.h"
#include "ConsoleWidget.h"
#include "DataWidget.h"
#include "Widget3D.h"
#include "window.h"
#include "SDataWidget.h"
#include "LogWidget.h"
#include "LogWidgetBasic.h"
#include <QLabel>
#include "EllipsoidCalibration.hpp"
#include "Globals.hpp"
#include "ArmTab.hpp"

extern DataRecorder* dr; // FIXME Eliminate this hideous workaround

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
        ui->setupUi(this);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));

        dr = &(app.dataRecorder);

        //Window *window = new Window(app);
        //ui->openglTab->layout()->addWidget(window);

        //Widget3D* widget3d = new Widget3D(ui->openglTab, app);
        //ui->openglTab->layout()->addWidget(widget3d);
        //ui->tabWidget->removeTab(8);    //temporarly hidden
        //ui->tabWidget->removeTab(6);
        //ui->tabWidget->removeTab(7);
        //ui->tabWidget->removeTab(3);

        LogWidget* logWidget = new LogWidget(ui->logTab, app);
        ui->logTab->layout()->addWidget(logWidget);

        LogWidgetBasic* logWidgetBasic = new LogWidgetBasic(ui->logTab, app);
        ui->basicLogTab->layout()->addWidget(logWidgetBasic);

        SDataWidget* sddataWidget = new SDataWidget(ui->sdataTab, app);
        ui->sdataTab->layout()->addWidget(sddataWidget);

	DataWidget* dataWidget = new DataWidget(ui->plotTab, app);
	ui->plotTab->layout()->addWidget(dataWidget);

        CalibrationWidget* calibrationWidget = new CalibrationWidget(ui->calibrationTab, app);
        ui->calibrationTab->layout()->addWidget(calibrationWidget);

	ConsoleWidget* consoleWidget = new ConsoleWidget(ui->consoleTab, app);
	ui->consoleTab->layout()->addWidget(consoleWidget);

        ui->ellipsoidTab->setLayout(new QVBoxLayout);
        EllipsoidCalibration* ellipsoid = new EllipsoidCalibration(ui->ellipsoidTab);
        ui->ellipsoidTab->layout()->addWidget(ellipsoid);

        ArmTab* armTab = new ArmTab(ui->armTab);
        ui->armTab->layout()->addWidget(armTab);

        statusBar()->showMessage("Started.");

        connect(&app.serialListener, SIGNAL(showNotification(const QString&)), statusBar(), SLOT(showMessage(const QString&)) );
        connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(const QString &)) );
        connect(&app, SIGNAL(showConsoleSignal(const QString &)), consoleWidget , SLOT(onRecieveConsoleSignal(const QString& )) );
        connect(calibrationWidget, SIGNAL(calibrationDone()), dataWidget, SLOT(newCalibrationOccured()) );
        connect(&(app.dataRecorder), SIGNAL(fileLoaded()), calibrationWidget, SLOT(OnFileLoaded()));
        connect(&(app.dataRecorder), SIGNAL(calibrationDataLoaded()), calibrationWidget, SLOT(OnFileLoaded()));
        //connect(dataWidget->plot, SIGNAL(angleChanged(double)), window, SLOT(onAngleChanged(double)));

        connect(dataWidget, SIGNAL(SolverStarted()), this, SLOT(onSolverRunning()));
        connect(dataWidget, SIGNAL(SolverFinished()), this, SLOT(onSolverRunning()));
        connect(&(app.tabWatcher), SIGNAL(SDCardTabGainedFocus()), sddataWidget, SLOT(onSdataLeftFocusIn()));
        //connect(ui->actionLog_Editable, SIGNAL(toggled(bool)), logWidgetE, SLOT(setTableEditable(bool)));

        connect(&app.serialListener,      SIGNAL(receiveMessage(ActiveMessage)),
                &app.timeSyncMsgReceiver, SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);

        app.tabWatcher.registerTabWidget(ui->tabWidget);

        connect(&app.serialListener,  SIGNAL(receiveMessage(const ActiveMessage& )),
                &app.connectionState, SLOT(msgReceived(const ActiveMessage& )));

        connect(&app.serialListener,  SIGNAL(receiveMessage(const ActiveMessage& )),
                &app.accelMagMsgReceiver, SLOT(onReceiveMessage(const ActiveMessage& )));

        globals::set_Ellipsoid(ellipsoid);
        globals::set_AccelMagMsgReceiver(&app.accelMagMsgReceiver);
        globals::connect_Ellipsoid_AccelMagMsgReceiver();

        connect(&app.serialListener,  SIGNAL(connected()),
                &app.connectionState, SLOT(connectedToPort()));

        connect(&app.serialListener,  SIGNAL(disconnected()),
                &app.connectionState, SLOT(disconnected()));

//        connect(&app.solver, SIGNAL(solver_done(bool,QString)),
//                logWidget,       SLOT(solverFinished(bool,QString)));

        ui->sdataTab->setFocusPolicy(Qt::StrongFocus);
        ui->sdataTab->installEventFilter(this);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
	ui->retranslateUi(this);
	break;
	default:
	break;
	}
}

void MainWindow::onSolverRunning()
{
    if(ui->calibrationTab->isEnabled()){
        ui->calibrationTab->setEnabled(false);
        ui->connectTab->setEnabled(false);
        ui->consoleTab->setEnabled(false);
        ui->graphTab->setEnabled(false);
    } else {
        ui->calibrationTab->setEnabled(true);
        ui->connectTab->setEnabled(true);
        ui->consoleTab->setEnabled(true);
        ui->graphTab->setEnabled(true);
    }

}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (object == ui->sdataTab)
        {
            emit focusIn();
        }
    }
    return false;
}
