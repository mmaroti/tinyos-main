#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"
#include <qextserialenumerator.h>
#include <QRadioButton>
#include <QListWidget>
#include <QList>
#include <QtDebug>
#include "Application.h"

ConnectWidget::ConnectWidget(QWidget *parent, Application &app) :
	QWidget(parent),
	ui(new Ui::ConnectWidget)
{
	ui->setupUi(this);

	QLayout *layout = ui->comPorts->layout();

	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	for (int i = 0; i < ports.size(); i++) {
/*
		qDebug() << "port name:" << ports.at(i).portName;
		qDebug() << "friendly name:" << ports.at(i).friendName;
		qDebug() << "physical name:" << ports.at(i).physName;
		qDebug() << "enumerator name:" << ports.at(i).enumName;
		qDebug() << "vendor ID:" << QString::number(ports.at(i).vendorID, 16);
		qDebug() << "product ID:" << QString::number(ports.at(i).productID, 16);
		qDebug() << "===================================";
*/
		QRadioButton *radio = new QRadioButton(ports.at(i).friendName);
		portButtons.insert(ports.at(i).portName, radio);
		layout->addWidget(radio);
	}

	QRadioButton *radio = new QRadioButton("Simulated Input Source");
	portButtons.insert("simulated", radio);
	layout->addWidget(radio);

	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addItem(spacer);

	moteIcon.addFile(":/icons/Wireless.png", QSize(), QIcon::Normal, QIcon::Off);

	connect(this, SIGNAL(portChanged(QString, int)), &app.serialListener, SLOT(onPortChanged(QString, int)));
	connect(&app.serialListener, SIGNAL(receiveMessage(const ActiveMessage &)), this, SLOT(onReceiveMessage(const ActiveMessage &)));
}

ConnectWidget::~ConnectWidget()
{
	delete ui;
}

void ConnectWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void ConnectWidget::on_connectButton_clicked()
{
	QString portName;

	foreach(QString port, portButtons.keys())
	{
		QRadioButton *button = portButtons.value(port);
		if( button && button->isChecked() )
			portName = port;
	}

	int baudRate = ui->baudRate->currentText().toInt();
	emit portChanged(portName, baudRate);
}

void ConnectWidget::on_disconnectButton_clicked()
{
	moteItems.clear();
	ui->motes->clear();

	emit portChanged("", 0);
}

void ConnectWidget::onReceiveMessage(const ActiveMessage & msg)
{
	if( ! moteItems.contains(msg.source) )
	{
		QListWidgetItem * item = new QListWidgetItem(ui->motes);
		item->setIcon(moteIcon);
		item->setText("Mote " + QString::number(msg.source));

		moteItems.insert(msg.source, item);
	}
}

void ConnectWidget::on_refreshButton_clicked()
{
	moteItems.clear();
	ui->motes->clear();
}
