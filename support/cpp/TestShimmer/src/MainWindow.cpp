#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include "RawDataWidget.h"
#include <QScrollArea>
#include "Application.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

//	QScrollArea * scrollArea = new QScrollArea();
//	scrollArea->setWidget(new PlotWidget(scrollArea));
//	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	ui->testTab->layout()->addWidget(scrollArea);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));
	ui->rawTab->layout()->addWidget(new RawDataWidget(ui->rawTab, app));

	statusBar()->showMessage("Started.");

	connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)));
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

