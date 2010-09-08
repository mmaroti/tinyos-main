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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include <QScrollArea>
#include "Application.h"
#include "CalibrationWidget.h"
#include "ConsoleWidget.h"
#include "DataWidget.h"
#include "GraphWidget.h"
#include "Widget3D.h"
#include "window.h"

extern DataWidget* dw;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));

	//Window *window = new Window(app);
	//ui->openglTab->layout()->addWidget(window);

//	Widget3D* widget3d = new Widget3D(ui->openglTab, app);
//	ui->openglTab->layout()->addWidget(widget3d);

	DataWidget* dataWidget = new DataWidget(ui->plotTab, app);
	ui->plotTab->layout()->addWidget(dataWidget);
        dw = dataWidget;

        GraphWidget* graphWidget = new GraphWidget(ui->graphTab, app);
        ui->graphTab->layout()->addWidget(graphWidget);

        CalibrationWidget* calibrationWidget = new CalibrationWidget(ui->calibrationTab, app);
        ui->calibrationTab->layout()->addWidget(calibrationWidget);

	ConsoleWidget* consoleWidget = new ConsoleWidget(ui->consoleTab, app);
	ui->consoleTab->layout()->addWidget(consoleWidget);

	statusBar()->showMessage("Started.");

        connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)) );
        connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(QString)) );
        connect(&app, SIGNAL(showConsoleSignal(const QString &)), consoleWidget , SLOT(onRecieveConsoleSignal(QString)) );
        connect(calibrationWidget, SIGNAL(calibrationDone()), dataWidget, SLOT(newCalibrationOccured()) );
        //connect(dataWidget->plot, SIGNAL(angleChanged(double)), window, SLOT(onAngleChanged(double)));
        connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
        connect(ui->actionImport, SIGNAL(triggered()), dataWidget, SLOT(on_loadBtn_clicked()));
        connect(ui->actionExport, SIGNAL(triggered()), dataWidget, SLOT(on_exportBtn_clicked()));

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

