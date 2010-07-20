#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include <QScrollArea>
#include "Application.h"
#include "CalibrationWidget.h"
#include "ConsoleWidget.h"
#include "DataWidget.h"
#include "Data3DWidget.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));

        Data3DWidget* data3dWidget = new Data3DWidget(ui->plot3dTab, app);
        ui->plot3dTab->layout()->addWidget(data3dWidget);

        DataWidget* dataWidget = new DataWidget(ui->plotTab, app);
        ui->plotTab->layout()->addWidget(dataWidget);

        CalibrationWidget* calibrationWidget = new CalibrationWidget(ui->calibrationTab, app);
        ui->calibrationTab->layout()->addWidget(calibrationWidget);

        ConsoleWidget* consoleWidget = new ConsoleWidget(ui->consoleTab, app);
        ui->consoleTab->layout()->addWidget(consoleWidget);

	statusBar()->showMessage("Started.");

        connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)) );
        connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(QString)) );
        connect(&app, SIGNAL(showConsoleSignal(const QString &)), consoleWidget , SLOT(onRecieveConsoleSignal(QString)) );
        connect(calibrationWidget, SIGNAL(calibrationDone()), dataWidget, SLOT(newCalibrationOccured()) );
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

