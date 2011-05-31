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
#include <QFileDialog>
#include "curvedata.h"
#include "Application.h"
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>

#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

MainWindow::MainWindow(QWidget *parent, Application &app):
    QMainWindow(parent),
    application(app)
{    
    d_plot = new Plot(this, application);

    setCentralWidget(d_plot);

    QToolBar *toolBar = new QToolBar(this);

    btnZoom = new QToolButton(toolBar);
    btnZoom->setText("Zoom");
    btnZoom->setCheckable(true);
    btnZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnZoom);    
    connect(btnZoom, SIGNAL(toggled(bool)), d_plot, SLOT(enableZoomMode(bool)));

    btnLoad = new QToolButton(toolBar);
    btnLoad->setText("Load");
    btnLoad->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnLoad);
    connect(btnLoad, SIGNAL(clicked()), SLOT(onLoadButtonPressed()));

    btnMarker = new QToolButton(this);
    btnMarker->setText("Marker");
    btnMarker->setCheckable(true);
    btnMarker->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnMarker);
    connect(btnMarker, SIGNAL(toggled(bool)), SLOT(enableMarkerMode(bool)));

    toolBar->addSeparator();

    this->addToolBar(toolBar);

    d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        d_plot->canvas());
    d_picker->setStateMachine(new QwtPickerDragPointMachine());
    d_picker->setRubberBandPen(QColor(Qt::green));
    d_picker->setRubberBand(QwtPicker::CrossRubberBand);
    d_picker->setTrackerPen(QColor(Qt::white));

    //connect(d_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButtonPressed()));
    //connect(d_clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonPressed()));
}

void MainWindow::onLoadButtonPressed()
{
    if(application.moteDataHolder.motesCount() != 0){
        const int ret = QMessageBox::warning(this, "Warning", "Loading new data will erase earlier ones.\nAre you sure?",
                                             QMessageBox::Yes, QMessageBox::Cancel);

        if(ret==QMessageBox::Yes){
            onClearButtonPressed();
        } else {
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this,"Select one or more files to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.moteDataHolder.loadCSVData( file );
    }
}

void MainWindow::onClearButtonPressed()
{
    d_plot->clearCurves();

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){
        delete application.moteDataHolder.mote(i);
    }

//    for(int i=0; i<curve_datas.size(); i++){
  //      delete curve_datas[i];
    //}

    application.moteDataHolder.clearMotes();
    curve_datas.clear();
}



void MainWindow::onLoadFinished()
{
    long int longestID = -1;
    int maxLength = 0;
    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
        if(application.moteDataHolder.mote(i)->getLength() > maxLength){
            maxLength = application.moteDataHolder.mote(i)->getLength();
            longestID = i;
        }
    }
    double time = application.moteDataHolder.mote(longestID)->getLength();

    int minValue, maxValue;
    maxValue = 0; minValue = 20000;

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){

        long int samplesSize = application.moteDataHolder.mote(i)->samplesSize();
        int interval = samplesSize/100;

        CurveData* curve_data = new CurveData;

        for(long int j=0; j < samplesSize-interval; j+=interval)
        {

            double value;
            value = application.moteDataHolder.mote(i)->sampleAt(j).xAccel + application.moteDataHolder.mote(i)->sampleAt(j).yAccel + application.moteDataHolder.mote(i)->sampleAt(j).zAccel;

            if(value > maxValue) maxValue = value;
            if(value < minValue) minValue = value;

            double time;
            time = application.moteDataHolder.mote(i)->sampleAt(j).unix_time;
            QPointF point(time, value);

            curve_data->append(point);
        }

        curve_datas.append(curve_data);

        d_plot->setMoteCurve(i);
    }

    d_plot->setAxisScale(QwtPlot::xBottom, -10, time+10);
    d_plot->setAxisScale(QwtPlot::yLeft, minValue, maxValue);
    d_plot->replot();

    d_plot->createZoomer();

}

void MainWindow::enableMarkerMode(bool)
{
    d_plot->enableZoomMode(false);
    btnZoom->setChecked(false);


}
