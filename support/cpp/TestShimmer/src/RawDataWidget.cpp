
#include "RawDataWidget.h"
#include "ui_RawDataWidget.h"
#include "RawDataPlot.h"
#include "Application.h"
#include "QtDebug"

RawDataWidget::RawDataWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::RawDataWidget),
    application(app)
{
	ui->setupUi(this);
	plot = new RawDataPlot(ui->scrollArea, app);
	ui->scrollArea->setWidget(plot);
}

RawDataWidget::~RawDataWidget()
{
	delete ui;
}

void RawDataWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void RawDataWidget::on_recordButton_clicked()
{
	QString text = ui->recordButton->text();
	if( text == "Record" )
	{
		ui->recordButton->setText("Stop");
		connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
	}
	else
	{
		ui->recordButton->setText("Record");
		disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
	}
}

void RawDataWidget::on_clearButton_clicked()
{
//	ui->recordButton->setText("Record");
//	disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));

	application.dataRecorder.clearMessages();
}

void RawDataWidget::on_xAccel_clicked()
{
	plot->setGraphs(RawDataPlot::XACCEL, ui->xAccel->checkState());
}

void RawDataWidget::on_yAccel_clicked()
{
	plot->setGraphs(RawDataPlot::YACCEL, ui->yAccel->checkState());
}

void RawDataWidget::on_zAccel_clicked()
{
	plot->setGraphs(RawDataPlot::ZACCEL, ui->zAccel->checkState());
}

void RawDataWidget::on_xGyro_clicked()
{
	plot->setGraphs(RawDataPlot::XGYRO, ui->xGyro->checkState());
}

void RawDataWidget::on_yGyro_clicked()
{
	plot->setGraphs(RawDataPlot::YGYRO, ui->yGyro->checkState());
}

void RawDataWidget::on_zGyro_clicked()
{
	plot->setGraphs(RawDataPlot::ZGYRO, ui->zGyro->checkState());
}

void RawDataWidget::on_voltage_clicked()
{
	plot->setGraphs(RawDataPlot::VOLTAGE, ui->voltage->checkState());
}
