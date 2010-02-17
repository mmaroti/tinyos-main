
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
	ui->scrollArea->setWidget(new RawDataPlot(ui->scrollArea, app));
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
