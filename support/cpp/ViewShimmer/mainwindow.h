#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qwidget.h>
#include <QHBoxLayout>
#include <QVector>
#include <QMainWindow>
#include <QToolButton>
#include "MoteData.h"

class CurveData;
class Plot;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotMarker;
class Application;

enum CursorMode{
    ZOOM,
    MARKER,
    CURSOR
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *, Application& app);

    CurveData* curve_data_at(int i){
            return curve_datas[i];
    }

    int curve_datas_size(){
        return curve_datas.size();
    }

    void calculateCurveDatas(double zoomRatio);
    void clearCurveDatas();


Q_SIGNALS:
    void signalIntervalChanged(double);

public Q_SLOTS:
    void onLoadButtonPressed();
    void onClearButtonPressed();
    void onLoadFinished();
    void enableZoomMode(bool);
    void enableMarkerMode(bool);
    void enableCopyMode(bool);
    //void enablePasteMode(bool);
    void enableCutMode(bool);
    void createMarker(const QPointF &  pos );
    void copy(QRectF);
    void cut(QRectF);
    void paste(QPointF);

private:
    Application &application;

    Plot *d_plot;

    QwtPlotPicker *d_picker;

    QToolButton *btnZoom;
    QToolButton *btnMarker;
    QToolButton *btnLoad;
    QToolButton *btnClear;
    QToolButton *btnSave;
    QToolButton *btnCopy;
    QToolButton *btnPaste;
    QToolButton *btnCut;

    QVector<CurveData*> curve_datas;
    QVector<QwtPlotMarker*> markers;

    QVector<int> copyPositions;
    QVector<Sample> copySamples;
};

#endif


