/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of the University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
* @author Andras Becsi (abecsi@inf.u-szeged.hu)
*/

#include "PlotWidget.h"

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <cstdlib>
#include <qwt_symbol.h>
#include <QTextStream>
#include <QList>
#include <QIODevice>
#include <QFileDialog>
#include <QMessageBox>

PlotWidget::PlotWidget(QWidget* parent):
        QwtPlot(parent),
        interval(0)
{
    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);
    updateTimer = new QTimer(this);
    connect(updateTimer,SIGNAL(timeout()), SLOT(advance()));
    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#ifdef Q_WS_X11
    /*
    Qt::WA_PaintOnScreen is only supported for X11, but leads
    to substantial bugs with Qt 4.2.x/Windows
    */
    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif

    alignScales();

    marker = new QwtPlotMarker();
    marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setXValue(BUFFER_SIZE/2);
    marker->hide();
    marker->attach(this);

    // Insert new curves
    curve = new QwtPlotCurve("Data");

    clearAndInit();
    // Assign a title
//    setTitle("A Test for plotting data");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);


    curve->attach(this);
    // Set curve styles
    curve->setPen(QPen(Qt::red));
    curve->setRawData(timeScale, displayBuffer, BUFFER_SIZE);

    //     QwtPlotMarker *mY = new QwtPlotMarker();
    //     mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    //     mY->setLineStyle(QwtPlotMarker::HLine);
    //     mY->setYValue(1.0);
    //     mY->attach(this);

    // Axis
    //setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    setAxisScale(QwtPlot::xBottom, 0, BUFFER_SIZE);
    enableAxis(QwtPlot::xBottom, false);
    setAxisTitle(QwtPlot::yLeft, "Érték");
    setAxisScale(QwtPlot::yLeft, Y_INTERVALL[0], Y_INTERVALL[1]);
    QwtSymbol sym;
    sym.setStyle(QwtSymbol::XCross);
    sym.setPen(QColor(Qt::blue));
    sym.setBrush(QColor(Qt::yellow));
    sym.setSize(3);
    curve->setSymbol(sym);
    setMargin(10);
}

void PlotWidget::alignScales() {
    canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(2);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, true);
    }
}

void PlotWidget::incomingData(double data) {
        incomingDataBuffer.append(data);

    emit sliderUpdateNeeded(dataPosition, incomingDataBuffer.size());
}

void PlotWidget::incomingData(const QList<double>& data) {
        incomingDataBuffer.append(data);

    emit sliderUpdateNeeded(dataPosition, incomingDataBuffer.size());
}

void PlotWidget::setTimerInterval(double ms) {
    interval = qRound(ms);
    updateTimer->setInterval(interval);
}

void PlotWidget::clearDisplay() {
    for (int i = 0; i< BUFFER_SIZE; i++) {
        displayBuffer[i] = 0;
    }
    dataPosition = 0;
    curve->hide();
    replot();
    curve->show();
}

void PlotWidget::advance() {
    if ( dataPosition < incomingDataBuffer.size() ) {
if (DEBUG_OUTPUT)    fprintf(stderr,"\n{{%d,pos=%d}}\n",incomingDataBuffer.size(),dataPosition);

        for ( int i = 0; i < BUFFER_SIZE - 1; i++ )
            displayBuffer[i] = displayBuffer[i+1];
        displayBuffer[BUFFER_SIZE - 1] = incomingDataBuffer[dataPosition];

        ++dataPosition;

        replot();
    }
    emit sliderUpdateNeeded(dataPosition);
}

void PlotWidget::pause() {
    updateTimer->stop();
}

void PlotWidget::resume() {
        updateTimer->start();
}

void PlotWidget::reset() {
    pause();
    clearDisplay();
    emit sliderUpdateNeeded(0);
}

bool PlotWidget::saveData(const QString& filename) {
    QFile backingStore(filename);
    if (!backingStore.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return false;

#ifdef Q_WS_WIN
    QString end = "\r\n";
#else
    QString end = "\n";
#endif
    QString date = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss");
    QTextStream out(&backingStore);

    for (int i = -PREAMBLE; i < dataExchangeBuffer.size(); i++) {
        out << date << "," << i << "," << ((i < 0)?0:dataExchangeBuffer[i]) << end;
    }
    out.flush();
    backingStore.close();
    return true;
}

bool PlotWidget::openData(const QString& filename) {
    QFile backingStore(filename);
    if (!backingStore.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    updateTimer->stop();
    QTextStream in(&backingStore);
    dataExchangeBuffer.clear();
    while (!in.atEnd()) {
        QStringList dataList = in.readLine().split(",");
        bool ok = false;
        double tmp = dataList.last().toDouble(&ok);
        if (ok) {
            dataExchangeBuffer.append(tmp);
        } else {
            dataExchangeBuffer.append(0.0);
        }

#ifdef DEBUG_OUTPUT
        fprintf(stderr, "ok=%d\n",ok);
        fprintf(stderr, "data=%f\n",tmp);
    }
#endif

    backingStore.close();
    clearDisplay();
    incomingDataBuffer.clear();
    incomingDataBuffer.append(dataExchangeBuffer);

#ifdef DEBUG_OUTPUT
    fprintf(stderr,"data_buffer_size=%d\n",incomingDataBuffer.size());
#endif
    emit sliderUpdateNeeded(0, incomingDataBuffer.size());
    emit dataImported();
    sliderValueChanged(dataPosition, true);
    return true;
}

void PlotWidget::fillExchangeBuffer() {
    dataExchangeBuffer.clear();
    for (int i = 0; i < incomingDataBuffer.size(); i++)
        dataExchangeBuffer.append(incomingDataBuffer[i]);
}

void PlotWidget::sliderValueChanged(double val, bool init) {
    clearDisplay();
    dataPosition = (val < incomingDataBuffer.size()) ? val :
                   ((incomingDataBuffer.isEmpty()) ? 0 : incomingDataBuffer.size()-1);
    displayBuffer[BUFFER_SIZE] = (incomingDataBuffer.isEmpty())?0:incomingDataBuffer[dataPosition];

    for (int i = 0, j = (dataPosition - BUFFER_SIZE/2); i < BUFFER_SIZE; i++, j++) {
        if (j < 0 || j >= incomingDataBuffer.size() || init)
            displayBuffer[i] = 0;
        else
            displayBuffer[i] = incomingDataBuffer[j];
    }
    if (dataPosition) setShowMarker(true);

    replot();
}

void PlotWidget::setShowMarker(bool on) {
    if (on) {
        marker->show();
    } else {
        marker->hide();
    }
}

void PlotWidget::clearAndInit() {
    reset();

    //  Initialize data
    for (int i = 0; i< BUFFER_SIZE; i++)
    {
        timeScale[i] = i;     // time axis
    }
    dataPosition = 0;
    incomingDataBuffer.clear();
    dataExchangeBuffer.clear();
    sliderUpdateNeeded(0,BUFFER_SIZE);
    setShowMarker(false);
    replot();
}

void PlotWidget::selectFile(bool save) {
    QString message;
    if (save) {
        fillExchangeBuffer();
        QString selectedFile = QFileDialog::getSaveFileName(this, "Adatsor mentése" , "Adatok", "*.csv", 0, QFileDialog::DontConfirmOverwrite);
        if (!saveData(selectedFile))
            message = "Fájl mentése sikertelen!";
    } else {
        QString selectedFile = QFileDialog::getOpenFileName(this, "Adatsor betöltése" , "Adatok", "*.csv");
        if (!QFile::exists(selectedFile) || !openData(selectedFile)) {
            message = "Fájl megnyitása sikertelen!";
        } else {
            pause();
        }
        if (!message.isEmpty())
            QMessageBox::critical(this,"Sikertelen fájlművelet", message);
    }
}
