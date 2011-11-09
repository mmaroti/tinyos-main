#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qwidget.h>
#include <QHBoxLayout>
#include <QVector>
#include <QMainWindow>
#include <QToolButton>
#include <QLineEdit>
#include "MoteData.h"

class CurveData;
class Plot;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotMarker;
class Application;
class QDockWidget;
class QListWidget;
class QListWidgetItem;
class QProgressBar;



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
    void clearCopyDatas();
    void clearMarkers();

Q_SIGNALS:
    void signalIntervalChanged(double);

public Q_SLOTS:
    void onLoadButtonPressed();
    void onSaveButtonPressed();
    void onClearButtonPressed();
    void onLoadFinished();
    void enableZoomMode(bool);
    void enableMarkerMode(bool);
    void enableCopyMode(bool);
    void enablePasteMode(bool);
    void enableCutMode(bool);
    void enableSDownloader();
    void enableOffsetMode(bool);
    void enableOnlineMode(bool);
    void createMarker(const QPointF &  pos );
    void createMarker(const QPointF & pos, QString text, QColor color = Qt::green);
    void copy(QRectF);
    void cut(QRectF);
    void paste(QPointF);

    //void calculateOffset(double time);

private slots:
    void on_listWidget_itemDoubleClicked( QListWidgetItem * item );
    void setOffset(const QPointF &  pos );

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
    QToolButton *btnSData;
    QToolButton *btnOffset;
    QToolButton *btnOnlineMode;

    QDockWidget *dockWidget;

    QDockWidget *dockWidget2;
    QLineEdit *markerText;
    QListWidget *listWidget;    

    QProgressBar* progressBar;

    QVector<CurveData*> curve_datas;
    QVector<QwtPlotMarker*> markers;

    QVector<int> copyPositions;

    void exitFailure(const QString& dir) const;
};

#endif


