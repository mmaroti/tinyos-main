#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include "RawDataWidget.h"
#include "CalibratedDataWidget.h"
#include <QScrollArea>
#include "Application.h"
#include "CalibrationWidget.h"
#include "ConsoleWidget.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));
	ui->rawTab->layout()->addWidget(new RawDataWidget(ui->rawTab, app));
        ui->calibratedTab->layout()->addWidget(new CalibratedDataWidget(ui->calibratedTab, app));
	ui->calibrationTab->layout()->addWidget(new CalibrationWidget(ui->calibrationTab, app));
        ConsoleWidget* consoleWidget = new ConsoleWidget(ui->consoleTab, app);
        ui->consoleTab->layout()->addWidget(consoleWidget);

	statusBar()->showMessage("Started.");

	connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)));
	connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(QString)));
        connect(&app, SIGNAL(showConsolSignal(const QString &)), consoleWidget , SLOT(on_recieveConsolSignal(QString)));
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

