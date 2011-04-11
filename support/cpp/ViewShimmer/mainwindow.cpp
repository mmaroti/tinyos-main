#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include "ConnectWidget.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent):
    QWidget(parent)
{

    const double intervalLength = 10.0; // seconds

    d_plot = new Plot(this);
    d_plot->setIntervalLength(intervalLength);

    d_intervalWheel = new WheelBox("Displayed [s]", 1.0, 100.0, 1.0, this);
    d_intervalWheel->setValue(intervalLength);

    d_connectButton = new QPushButton("Disconnect", this);

    QLabel* time_lbl = new QLabel(this);
    QLabel* samples_lbl = new QLabel(this);
    time = new QLabel(this);
    samples = new QLabel(this);

    time_lbl->setText("Time:");
    samples_lbl->setText("Samples:");

    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget(d_intervalWheel);
    vLayout1->addStretch(10);
    vLayout1->addWidget(d_connectButton);
    vLayout1->addWidget(samples_lbl);
    vLayout1->addWidget(samples);
    vLayout1->addWidget(time_lbl);
    vLayout1->addWidget(time);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d_plot, 10);    
    layout->addLayout(vLayout1);


    connect(d_intervalWheel, SIGNAL(valueChanged(double)),
        d_plot, SLOT(setIntervalLength(double)) );
    connect(d_connectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonPressed()));
}

void MainWindow::start()
{
    d_plot->start();
}

void MainWindow::setTime(QString t){
    time->setText(t);
}

void MainWindow::setSamples(QString s){
    samples->setText(s);
}

void MainWindow::onConnectButtonPressed()
{
    if(d_connectButton->text() == "Disconnect"){
        d_connectButton->setText("Connect");
        d_plot->stop();
    } else {
        d_connectButton->setText("Disconnect");
        d_plot->start();
    }


}
