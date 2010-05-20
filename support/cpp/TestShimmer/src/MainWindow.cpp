#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ConnectWidget.h"
#include "RawDataWidget.h"
#include "CalibratedDataWidget.h"
#include <QScrollArea>
#include "Application.h"
#include "CalibrationWidget.h"
#include "ConsoleWidget.h"
#include "LinearEquations.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->connectTab->layout()->addWidget(new ConnectWidget(ui->connectTab, app));
	ui->rawTab->layout()->addWidget(new RawDataWidget(ui->rawTab, app));
        ui->calibratedTab->layout()->addWidget(new CalibratedDataWidget(ui->calibratedTab, app));
	ui->calibrationTab->layout()->addWidget(new CalibrationWidget(ui->calibrationTab, app));
        ui->consoleTab->layout()->addWidget(new ConsoleWidget(ui->consoleTab, app));

	statusBar()->showMessage("Started.");

	connect(&app.serialListener, SIGNAL(showNotification(const QString &, int)), statusBar(), SLOT(showMessage(const QString &, int)));
	connect(&app, SIGNAL(showMessageSignal(const QString &)), statusBar(), SLOT(showMessage(QString)));

        /*LinearEquations linearEquations;

        Equation* equation1 = linearEquations.createEquation();
        Equation* equation2 = linearEquations.createEquation();
        Equation* equation3 = linearEquations.createEquation();

        equation1->setConstant(20);
        equation1->setCoefficient("x", 1);
        equation1->setCoefficient("y", 1);
        linearEquations.addEquation(equation1);

        equation2->setConstant(10);
        equation2->setCoefficient("x", 1);
        equation2->setCoefficient("y", -1);
        linearEquations.addEquation(equation2);

        equation3->setConstant(25);
        equation3->setCoefficient("x", 1);
        equation3->setCoefficient("y", 2);
        linearEquations.addEquation(equation3);

        linearEquations.printStatistics();

        Solution* solution = linearEquations.solveWithSVD(0.1);
        solution->print();
        linearEquations.clear();*/
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

