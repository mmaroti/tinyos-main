#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qwidget.h>
#include <QHBoxLayout>
#include <QVector>
#include <QMainWindow>
#include <QToolButton>

class CurveData;
class Plot;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class Application;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *, Application& app);

    CurveData* curve_data_at(int i){
            return curve_datas[i];
    }


Q_SIGNALS:
    void signalIntervalChanged(double);

public Q_SLOTS:
    void onLoadButtonPressed();
    void onClearButtonPressed();
    void onLoadFinished();
    void enableMarkerMode(bool);

private:
    Application &application;

    Plot *d_plot;

    QwtPlotPicker *d_picker;

    QToolButton *btnZoom;
    QToolButton *btnMarker;
    QToolButton *btnLoad;

    QVector<CurveData*> curve_datas;
};

#endif


