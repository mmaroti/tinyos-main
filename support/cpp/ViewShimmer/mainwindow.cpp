#include "mainwindow.h"
#include "plot.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include "curvedata.h"
#include "Application.h"
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QClipboard>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QDir>
#include <QDialog>

#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_widget.h>

#include "MoteData.h"
#include "constants.h"

#include "scrollbar.h"

MainWindow::MainWindow(QWidget *parent, Application &app):
    QMainWindow(parent),
    application(app)
{    
    d_plot = new Plot(this, application);

    setCentralWidget(d_plot);

    onlineCurve = new CurveData();
    curve_datas.append(onlineCurve);
    d_plot->setMoteCurve(-1);
    scrollBar = new ScrollBar();

    QToolBar *toolBar = new QToolBar(this);    

    btnLoad = new QToolButton(toolBar);
    btnLoad->setText("Load");
    btnLoad->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnLoad);
    connect(btnLoad, SIGNAL(clicked()), SLOT(onLoadButtonPressed()));

    btnClear = new QToolButton(toolBar);
    btnClear->setText("Clear");
    btnClear->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnClear);
    connect(btnClear, SIGNAL(clicked()), SLOT(onClearButtonPressed()));

    btnSave = new QToolButton(this);
    btnSave->setText("Save");
    toolBar->addWidget(btnSave);
    connect(btnSave, SIGNAL(clicked()), SLOT(onSaveButtonPressed()));

    toolBar->addSeparator();

    btnZoom = new QToolButton(toolBar);
    btnZoom->setText("Zoom");
    btnZoom->setCheckable(true);
    btnZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnZoom);    
    connect(btnZoom, SIGNAL(toggled(bool)), SLOT(enableZoomMode(bool)));

    btnMarker = new QToolButton(this);
    btnMarker->setText("Marker");
    btnMarker->setCheckable(true);
    btnMarker->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnMarker);
    connect(btnMarker, SIGNAL(toggled(bool)), SLOT(enableMarkerMode(bool)));

    btnCut = new QToolButton(this);
    btnCut->setText("Cut");
    btnCut->setCheckable(true);
    toolBar->addWidget(btnCut);    
    connect(btnCut, SIGNAL(toggled(bool)), SLOT(enableCutMode(bool)));

    btnCopy = new QToolButton(this);
    btnCopy->setText("Copy");
    btnCopy->setCheckable(true);
    toolBar->addWidget(btnCopy);
    connect(btnCopy, SIGNAL(toggled(bool)), SLOT(enableCopyMode(bool)));

    btnPaste = new QToolButton(this);
    btnPaste->setText("Paste");    
    btnPaste->setCheckable(true);
    toolBar->addWidget(btnPaste);
    connect(btnPaste, SIGNAL(toggled(bool)), SLOT(enablePasteMode(bool)));

    btnOnlineMode = new QToolButton(this);
    btnOnlineMode->setText("Online Mode");
    btnOnlineMode->setCheckable(true);
    toolBar->addWidget(btnOnlineMode);
    connect(btnOnlineMode, SIGNAL(toggled(bool)), SLOT(enableOnlineMode(bool)));

    toolBar->addSeparator();

    bool success = QDir::setCurrent("rec");

    if (!success) {

        exitFailure("rec");
    }

    qDebug() << "Working directory is " << QDir::currentPath();
    btnSData = new QToolButton(this);
    btnSData->setText("SData Downloader");
    toolBar->addWidget(btnSData);
    connect(btnSData, SIGNAL(clicked()), SLOT(enableSDownloader()));

    toolBar->addSeparator();

    btnOffset = new QToolButton(this);
    btnOffset->setText("Set Offset");
    btnOffset->setCheckable(true);
    toolBar->addWidget(btnOffset);
    connect(btnOffset, SIGNAL(toggled(bool)), SLOT(enableOffsetMode(bool)));

    toolBar->addSeparator();

    this->addToolBar(toolBar);

    d_picker = new QwtPlotPicker(d_plot->canvas());
    d_picker->setTrackerMode(QwtPicker::AlwaysOn);
    d_picker->setTrackerPen(QColor(Qt::white));

    copyPositions.clear();

    markerText = new QLineEdit(this);
    markerText->hide();
    dockWidget = new QDockWidget(tr("Marker Notes"), this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
    dockWidget->setWidget(markerText);
    addDockWidget(Qt::TopDockWidgetArea, dockWidget);

    dockWidget->hide();

    listWidget = new QListWidget(this);

    dockWidget2 = new QDockWidget(tr("List of markers"), this);
    dockWidget2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
    dockWidget2->setWidget(listWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget2);

    dockWidget2->hide();

    btnClear->setDisabled(true);
    btnSave->setDisabled(true);
    btnCopy->setDisabled(true);
    btnCut->setDisabled(true);
    btnPaste->setDisabled(true);
    btnZoom->setDisabled(true);
    btnMarker->setDisabled(true);


    //connect(d_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButtonPressed()));
    //connect(d_clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonPressed()));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_listWidget_itemDoubleClicked(QListWidgetItem*)));
}

void MainWindow::onLoadButtonPressed()
{
    if(application.moteDataHolder.motesCount() != 0){
        const int ret = QMessageBox::warning(this, "Warning", "Loading new data will erase earlier ones.\nAre you sure?",
                                             QMessageBox::Yes, QMessageBox::Cancel);

        if(ret==QMessageBox::Yes){
            //onClearButtonPressed();
        } else {
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this,"Select one or more files to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {

//        progressBar = new QProgressBar(this);
//        progressBar->setMinimum(0);
//        progressBar->setMaximum();

        application.moteDataHolder.loadCSVData( file );


        //load markers
        QFile f( file );
        QString line;

        if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
        {
            QTextStream ts( &f );
            line = ts.readLine();

            while(line != "#marker_id,marker_text,marker_x_pos" && !ts.atEnd() ){
                line = ts.readLine();
            }

            line = ts.readLine();

            while ( !line.isEmpty() )
            {
                QStringList list = line.split(",");
                QStringListIterator csvIterator(list);

                QString text;
                QPointF pos;
                if(csvIterator.hasNext()){
                    int id = csvIterator.next().toInt();
                    text = csvIterator.next();
                    double xPos = csvIterator.next().toDouble();
                    double yPos = csvIterator.next().toDouble();

                    pos.setX(xPos);
                    pos.setY(yPos);
                }

                createMarker(pos, text);
                line = ts.readLine();
            }
        }

        btnClear->setEnabled(true);
        btnSave->setEnabled(true);
        btnCopy->setEnabled(true);
        btnCut->setEnabled(true);
        btnPaste->setEnabled(true);
        btnZoom->setEnabled(true);
        btnMarker->setEnabled(true);
        btnOffset->setEnabled(true);
    }
}

void MainWindow::onSaveButtonPressed()
{
    QString file = QFileDialog::getSaveFileName(this,"Select a file to save to!", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.moteDataHolder.saveData( file );

        QFile f( file );

        if( !f.open( QIODevice::Append) )
          {
              return;
          }

        QTextStream ts( &f );

        ts << "#marker_id,marker_text,marker_x_pos" << endl;
        for (int i = 0; i < markers.size(); i++){
          ts << i << "," << markers[i]->label().text() << "," << markers[i]->xValue() << "," << markers[i]->yValue() << endl;
        }

        ts.flush();
        f.close();

        btnZoom->setEnabled(true);
        btnMarker->setEnabled(true);
        btnClear->setEnabled(true);
    }

}

void MainWindow::clearMarkers()
{
    for(int i = 0; i < markers.size(); i++){
        markers[i]->detach();
    }
    markers.clear();
}

void MainWindow::onClearButtonPressed()
{
    btnMarker->setDisabled(true);
    btnZoom->setDisabled(true);

    d_plot->enableZoomMode(false);
    //d_plot->deleteZoomer();
    clearMarkers();
    clearCurveDatas();

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){
        delete application.moteDataHolder.mote(i);
    }

    application.moteDataHolder.clearMotes();

    d_plot->replot();

    btnMarker->setChecked(false);
    btnCopy->setChecked(false);
    btnCut->setChecked(false);
    btnPaste->setChecked(false);

    btnCopy->setDisabled(true);
    btnCut->setDisabled(true);
    btnClear->setDisabled(true);
    btnSave->setDisabled(true);
}

void MainWindow::clearCurveDatas()
{
    d_plot->clearCurves();

    curve_datas.clear();
}

void MainWindow::clearCopyDatas()
{
    copyPositions.clear();

}



void MainWindow::onLoadFinished()
{

    calculateCurveDatas(1.0);
    d_plot->createZoomer();

}

void MainWindow::enableZoomMode(bool on)
{
    d_plot->enableZoomMode(on);
    if(on == true){
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);
        btnOffset->setChecked(false);
    }
    d_picker->setEnabled(!on);
}

void MainWindow::enableMarkerMode(bool on)
{
    //d_plot->enableZoomMode(false);

    if(on == true){

        dockWidget->show();
        dockWidget2->show();
        markerText->setFocus();

        btnZoom->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);        

        d_picker->setRubberBand(QwtPlotPicker::VLineRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragPointMachine());

        connect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(createMarker(QPointF)));
    } else {
        dockWidget->hide();
        dockWidget2->hide();
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(createMarker(QPointF)));
    }
}

void MainWindow::enableOffsetMode(bool on)
{
    if(on == true){

        d_plot->enableZoomMode(true);

        btnMarker->setChecked(false);
        btnZoom->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::VLineRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragPointMachine());

        connect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(setOffset(QPointF)));
    } else {
        d_plot->enableZoomMode(false);
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(setOffset(QPointF)));
    }
}

void MainWindow::enableCopyMode(bool on)
{

    //d_plot->enableZoomMode(false);

    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::RectRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragRectMachine());

        connect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(copy(QRectF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(copy(QRectF)));
    }
}

void MainWindow::enableCutMode(bool on)
{

    //d_plot->enableZoomMode(false);

    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnPaste->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::RectRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragRectMachine());

        connect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(cut(QRectF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(cut(QRectF)));
    }
}

void MainWindow::enablePasteMode(bool on)
{
    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragPointMachine());

        connect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(paste(QPointF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(paste(QPointF)));
    }
}

void MainWindow::enableSDownloader()
{
    application.sdataWidget.show();
}

void MainWindow::enableOnlineMode(bool on)
{
    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);

        application.connectWidget.show();
    } else {

    }
}


void MainWindow::calculateCurveDatas(double zoomRatio)
{
    clearCurveDatas();

    qDebug() << "PLOT DEBUG:";
    qDebug() << "Zoom ratio: " << zoomRatio;

    //long int longestID = -1;
    //int maxLength = 0;
    double endTime = 0;
    double startTime = 999999999.99;
    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
        /*if(application.moteDataHolder.mote(i)->getLength() > maxLength){
            maxLength = application.moteDataHolder.mote(i)->getLength();
            longestID = i;
        }*/

        long int last = application.moteDataHolder.mote(i)->samplesSize()-1;
        double lastTime = application.moteDataHolder.mote(i)->sampleAt(last).unix_time;
        qDebug() << "last: " << last << "; lastTime: " << lastTime;

        if(endTime < lastTime) endTime = lastTime;
        if(startTime > application.moteDataHolder.mote(i)->sampleAt(1).unix_time) startTime = application.moteDataHolder.mote(i)->sampleAt(1).unix_time;
    }
    //double time = application.moteDataHolder.mote(longestID)->getLength();

    int minValue, maxValue;
    maxValue = 0; minValue = 20000;

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){

        int samplesSize = application.moteDataHolder.mote(i)->samplesSize();

        int interval = (samplesSize/d_plot->canvas()->width()*zoomRatio) + 1;
        qDebug() << "Point interval: " << interval;

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

    qDebug() << "startTime: " << startTime << "; endTime: " << endTime;
    d_plot->setAxisScale(QwtPlot::xBottom, -10-fabs(startTime), endTime+10);
    d_plot->setAxisScale(QwtPlot::yLeft, minValue, maxValue);
    d_plot->replot();

    //d_plot->createZoomer();


    qDebug() << "Canvas width: " << d_plot->canvas()->width() << ";";
    qDebug() << "Number of curve points: " << curve_datas[0]->size();
}

void MainWindow::onOnlineSampleAdded()
{
    double value;
    value = application.dataRecorder.at( application.dataRecorder.size()-1 ).xAccel;
    double time;
    time = application.dataRecorder.size()-1;

    if(time >= 20)    d_plot->createZoomer();

    QPointF point(time, value);

    onlineCurve->append(point);

    d_plot->replot();
}

void MainWindow::createMarker(const QPointF &pos)
{
    QString label = markerText->text()+"; ";
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setValue(pos);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker->setLinePen(QPen(Qt::green, 0, Qt::DashDotLine));

    label.append(QString::number(pos.x()));
    marker->setLabel(label);
    marker->attach(d_plot);

    d_plot->replot();

    markers.append(marker);
    markerText->clear();
    markerText->setFocus();

    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(label);
    listWidget->insertItem(markers.size(), newItem);
}

void MainWindow::createMarker(const QPointF &pos, QString text, QColor color)
{
    QString label = text;
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setValue(pos);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker->setLinePen(QPen(color, 0, Qt::DashDotLine));

    marker->setLabel(label);
    marker->attach(d_plot);

    d_plot->replot();

    markers.append(marker);
    markerText->clear();
    markerText->setFocus();
}


void MainWindow::copy(QRectF rect)
{
    double from = rect.bottomLeft().x();
    double to = rect.bottomRight().x();

    clearCopyDatas();

    qDebug() << "================";
    qDebug() << "Copy Positions";

    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){

        int begining = application.moteDataHolder.findNearestSample(from, i);
        qDebug() << "Copy times: begining: " << begining << " , unix_time@begining: " << application.moteDataHolder.mote(i)->sampleAt(begining).unix_time << ", unix_time@sample(0): " << application.moteDataHolder.mote(i)->sampleAt(0).unix_time << " ; real time: " << from;
        copyPositions.append(begining);

        int end = application.moteDataHolder.findNearestSample(to, i);
        qDebug() << end << " , " << application.moteDataHolder.mote(i)->sampleAt(end).unix_time << "; real time: " << to;
        copyPositions.append(end);
    }

    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;

    clipboardText.append("#mote,reboot_ID,length,boot_unix_time,skew_1,offset\n");

    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        QString row = application.moteDataHolder.mote(j)->getMoteHeader()+"\n";
        clipboardText.append(row);
    }

    clipboardText.append("#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp\n");

    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){
            QString row = QString::number(application.moteDataHolder.mote(j)->getMoteID())+","+QString::number(application.moteDataHolder.mote(j)->getRebootID())+","+application.moteDataHolder.mote(j)->sampleAt(i).toCsvString()+"\n";
            clipboardText.append(row);
        }
    }

    clipboardText.append("#marker_id,marker_text,marker_x_pos\n");

    for(int i = 0; i < markers.size(); i++){
        if(markers[i]->xValue() < to && markers[i]->xValue() >= from){
            QString row = QString::number(i)+","+markers[i]->label().text()+","+QString::number(markers[i]->xValue())+"\n";
            clipboardText.append(row);
        }
    }

    clipboard->setText(clipboardText);

}

void MainWindow::cut(QRectF rect)
{
    double from = rect.bottomLeft().x();
    double to = rect.bottomRight().x();

    int begining, end;
    clearCopyDatas();

    qDebug() << "================";
    qDebug() << "Copy Positions";
    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
        begining = application.moteDataHolder.findNearestSample(from, i);
        qDebug() << begining << " , " << application.moteDataHolder.mote(i)->sampleAt(begining).unix_time << "; real time: " << from;
        copyPositions.append(application.moteDataHolder.findNearestSample(from, i));


        end = application.moteDataHolder.findNearestSample(to, i);
        qDebug() << end << " , " << application.moteDataHolder.mote(i)->sampleAt(end).unix_time << "; real time: " << to;
        copyPositions.append(application.moteDataHolder.findNearestSample(to, i));
    }

    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;

    clipboardText.append("#mote,reboot_ID,length,boot_unix_time,skew_1,offset\n");

    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        QString row = application.moteDataHolder.mote(j)->getMoteHeader()+"\n";
        clipboardText.append(row);
    }

    clipboardText.append("#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp\n");

    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){
            QString row = QString::number(application.moteDataHolder.mote(j)->getMoteID())+","+QString::number(application.moteDataHolder.mote(j)->getRebootID())+","+application.moteDataHolder.mote(j)->sampleAt(i).toCsvString()+"\n";
            clipboardText.append(row);
        }
        application.moteDataHolder.mote(j)->deleteSamplesFrom(copyPositions.at(2*j), copyPositions.at(2*j+1)-copyPositions.at(2*j));
    }

    clipboardText.append("#marker_id,marker_text,marker_x_pos");

    for(int i = 0; i < markers.size(); i++){
        if(markers[i]->xValue() > to && markers[i]->xValue() <= from){
            QString row = QString::number(i)+","+markers[i]->label().text()+","+QString::number(markers[i]->xValue())+"\n";
            clipboardText.append(row);
        }
    }


    clipboard->setText(clipboardText);

    calculateCurveDatas(1.0);

}

void MainWindow::paste(QPointF pos)
{

    //paste from clipboard
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    if(!text.isNull()){

        qDebug() << "pasting from clipboard...";
        QTextStream stream(&text, QIODevice::ReadOnly);

        QString line = stream.readLine();
        if(line != "#mote,reboot_ID,length,boot_unix_time,skew_1,offset"){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format! Wrong header!");
            msgBox.exec();
        } else {
            line = stream.readLine();

            //create moteData from header information
            while ( !line.isEmpty() && line != "#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp" )
            {
                application.moteDataHolder.createMoteData(line);            //convert line string to mote header data
                line = stream.readLine();         // line of text excluding '\n'
            }

            //skip empty lines
            while( line != "#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp" )
            {
                line = stream.readLine();
            }



            double from = pos.x();
            if( from < d_plot->axisInterval(QwtPlot::xBottom).minValue() + BORDER_MARGIN ) from = d_plot->axisInterval(QwtPlot::xBottom).minValue();

            int begining;
            double minTime = 99999.999;
            double maxTime = 0.000;
            double timeDelay;

            for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
                for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){
                    if(minTime > application.moteDataHolder.mote(j)->sampleAt(i).unix_time) minTime = application.moteDataHolder.mote(j)->sampleAt(i).unix_time;
                    if(maxTime < application.moteDataHolder.mote(j)->sampleAt(i).unix_time) maxTime = application.moteDataHolder.mote(j)->sampleAt(i).unix_time;
                }
            }

            timeDelay = maxTime - minTime;
            qDebug() << "-----PASTE-----";
            qDebug() << "Paste time delay: " << minTime << " - " << maxTime;

            createMarker(QPointF(from, 0),"paste from", Qt::red);
            createMarker(QPoint(from+timeDelay,0), "paste to", Qt::red);

            line = stream.readLine();

            Sample sample = application.moteDataHolder.createSample(line, false);

            QVector<Sample> samples;
            QVector< QVector <Sample> > moteSamples;

            int prevMoteID = sample.moteID;
            int mote = 0;

            //create the samples for the actual moteData
            while ( !line.isEmpty() && line != "#marker_id,marker_text,marker_x_pos" )
            {
                Sample sample = application.moteDataHolder.createSample(line, false);

                if(prevMoteID == sample.moteID){
                    samples.append(sample);
                } else  {
                    moteSamples.append(samples);
                    samples.clear();
                    mote++;
                    samples.append(sample);
                }

                prevMoteID = sample.moteID;

                line = stream.readLine();         // line of text excluding '\n'

            }

            moteSamples.append(samples);

            qDebug() << "MoteSamples: " << moteSamples.size();

            for(int j = 0; j < application.moteDataHolder.motesCount(); j++){

                begining = application.moteDataHolder.findNearestSample(from, j);

                qDebug() << "Copy begining: " << begining;

                qDebug() << "Samples size: " << application.moteDataHolder.mote(j)->samplesSize();

                application.moteDataHolder.mote(j)->insertBlankSamples(begining, moteSamples[j].size());

                qDebug() << "   ...after injecting blank samples: " << application.moteDataHolder.mote(j)->samplesSize();

                qDebug() << "Time of first sample after blanks: " << application.moteDataHolder.mote(j)->sampleAt(begining+moteSamples[j].size()+1).unix_time;

                application.moteDataHolder.mote(j)->setTimeDelay(timeDelay, begining+moteSamples[j].size());

                qDebug() << "   ...after setting delyay: " << application.moteDataHolder.mote(j)->sampleAt(begining+moteSamples[j].size()+1).unix_time;

                for(int k = 0; k < moteSamples[j].size(); k++){
                    int pos = begining++;
                    Sample *sample = &application.moteDataHolder.mote(j)->getSampleAt(pos);
                    sample->unix_time = from+k*0.005;
                    sample->xAccel = moteSamples[j].at(k).xAccel;
                    sample->yAccel = moteSamples[j].at(k).yAccel;
                    sample->zAccel = moteSamples[j].at(k).zAccel;
                    sample->xGyro  = moteSamples[j].at(k).xGyro;
                    sample->yGyro  = moteSamples[j].at(k).yGyro;
                    sample->zGyro  = moteSamples[j].at(k).zGyro;

                    sample->counter = moteSamples[j].at(k).counter;
                    sample->mote_time = moteSamples[j].at(k).mote_time;

                }

            }

            line = stream.readLine();

            //load markers
            while ( !line.isEmpty() )
            {
                QStringList list = line.split(",");
                QStringListIterator csvIterator(list);

                QString text;
                QPointF pos;
                if(csvIterator.hasNext()){
                    int id = csvIterator.next().toInt();
                    text = csvIterator.next();
                    double xPos = csvIterator.next().toDouble();
                    double yPos = csvIterator.next().toDouble();

                    pos.setX(xPos);
                    pos.setY(yPos);
                }

                createMarker(pos, text);
                line = stream.readLine();
            }

        }


        onLoadFinished();
        return;
    }


    calculateCurveDatas(1.0);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int row = item->listWidget()->row(item);

    listWidget->takeItem(row);

    markers.at(row)->detach();
    markers.remove(row);

    d_plot->replot();
}

void MainWindow::exitFailure(const QString& dir) const {

    QString msg("Error: failed to set directory ");

    msg.append(dir);
    msg.append("\nRunning from:\n");
    msg.append(QDir::currentPath());

    QMessageBox mbox;

    mbox.setText(msg);
    mbox.exec();

    exit(EXIT_FAILURE);
}

void MainWindow::setOffset(const QPointF &pos)
{
    QMessageBox msgBox;
    msgBox.setText("Offset location set.");
    msgBox.setInformativeText("Are you sure this is the right sample?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Yes){
        int offsetSample = application.moteDataHolder.findNearestSample((double)pos.x(),0);
        application.moteDataHolder.calculateOffset(offsetSample);
        d_plot->replot();

        //enableOffsetMode(false);
        btnOffset->setChecked(false);
    } else if( ret == QMessageBox::No){
        qDebug() << "no";
    }

}
