#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include "RawDataWidget.h"
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
	ui->calibrationTab->layout()->addWidget(new CalibrationWidget(ui->calibrationTab, app));
        ui->consoleTab->layout()->addWidget(new ConsoleWidget(ui->consoleTab, app));

	statusBar()->showMessage("Started.");

	connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)));
	connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(QString)));
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

