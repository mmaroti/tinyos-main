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
        ui(new Ui::ConnectWidget),
        application(app)
{
        ui->setupUi(this);

        rescanPorts();

        moteIcon.addFile(":/icons/Wireless.png", QSize(), QIcon::Normal, QIcon::Off);

        connect(this, SIGNAL(portChanged(QString, int)), &app.serialListener, SLOT(onPortChanged(QString, int)));
        connect(&app.serialListener, SIGNAL(receiveMessage(const ActiveMessage &)), this, SLOT(onReceiveMessage(const ActiveMessage &)));
}

void ConnectWidget::rescanPorts()
{
        QLayout *layout = ui->comPorts->layout();

        portButtons.clear();
        QLayoutItem *child;
        while( (child = layout->takeAt(0)) != NULL )
        {
                QWidget *widget = child->widget();
                delete child;
                delete widget;
        }

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

                if( application.settings.value("port", "") == ports.at(i).portName )
                        radio->setChecked(true);
        }

        QRadioButton *radio = new QRadioButton("Simulated Input Source");
        portButtons.insert("simulated", radio);
        layout->addWidget(radio);

        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer);

        QString baud = application.settings.value("baudrate", "").value<QString>();
        for(int i = 0; i < ui->baudRate->count(); ++i)
        {
                if( ui->baudRate->itemText(i) == baud )
                        ui->baudRate->setCurrentIndex(i);
        }
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

        application.settings.setValue("port", portName);
        application.settings.setValue("baudrate", ui->baudRate->currentText());

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
        rescanPorts();
        moteItems.clear();
        ui->motes->clear();
}
