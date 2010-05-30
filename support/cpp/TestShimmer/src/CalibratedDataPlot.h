#ifndef CALIBRATEDDATAPLOT_H
#define CALIBRATEDDATAPLOT_H

#include <QWidget>
#include <QPen>
#include "cmath"
#include "CalibrationModule.h"

class PlotScrollArea;
class Application;
class DataRecorder;
class QPen;
class QTimerEvent;

class CalibratedDataPlot : public QWidget
{
        Q_OBJECT
public:
        CalibratedDataPlot(PlotScrollArea *parent, Application &app);

        enum
        {
                XACCEL = 0x0001,
                YACCEL = 0x0002,
                ZACCEL = 0x0004,
                AVGACC = 0x0008,
                XYANGLE = 0x0010,
                YZANGLE = 0x0020,
                ZXANGLE = 0x0040,
                GRID = 0x0100,
                TIME = 0x0200
        };

        void setGraphs(int graphs, bool on);
        int getGraphs() const { return graphs; }
        double calibrationDataAt(int i) { return calibrationData[i]; }
        double calculateAngle( double acceleration1, double acceleration2 );

protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent * event);

        int graphs;

public slots:
        void onSampleAdded();
        void onSamplesCleared();

private:
        Application &application;
        PlotScrollArea *scrollArea;
        int parentHeight;
        int plotWidth;

        double calibrationData[12];
        QPoint getPoint(int x, int y);
        QPoint getSample(int x, int y);
};

#endif // CALIBRATEDDATAPLOT_H
