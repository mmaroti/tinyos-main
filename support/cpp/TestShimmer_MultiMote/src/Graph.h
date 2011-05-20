#ifndef GRAPH_H
#define GRAPH_H
#define RADIAN 57.2957795

#include <QWidget>
#include <QPen>

class Application;
class QPen;
class DataRecorder;
class PlotScrollArea;

class Graph : public QWidget {
    Q_OBJECT

public:
        Graph(PlotScrollArea *parent, Application &app);

        enum graphs
        {
                XRAWACC   = 0x0001,
                YRAWACC   = 0x0002,
                ZRAWACC   = 0x0004,
                XRAWGYRO  = 0x0008,
                YRAWGYRO  = 0x0010,
                ZRAWGYRO  = 0x0020,
                XACC      = 0x0040,
                YACC      = 0x0080,
                ZACC      = 0x0100,
                XGYRO     = 0x0200,
                YGYRO     = 0x0400,
                ZGYRO     = 0x0800,
                ABSACC    = 0x1000,
                XYANG     = 0x2000,
                YZANG     = 0x4000,
                ZXANG     = 0x8000,

                VOLTAGE = 0x10000,
                GRID = 0x20000,
                TIME = 0x40000,
                TEMP = 0x80000,

                XANG      = 0x100000,
                YANG      = 0x200000,
                ZANG      = 0x400000,
        };

        void setGraphs(int graphs, bool on);
        int getGraphs() const { return graphs; }

        double calibrationDataAt(int i) { return calibrationData[i]; }

        double gyroCalibrationDataAt(int i) { return gyroCalibrationData[i]; }
        double calculateAngle( double acceleration1, double acceleration2 );
        double calculateCalibratedValue( QString axis, int time );
        void loadSettingsData();
        void onNewCalibration();

protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent * event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

        int graphs;

public slots:
        void onSampleAdded();
        void onSamplesCleared();

signals:
        void angleChanged(double angle);
        void clicked();

private:
        Application &application;
        PlotScrollArea *scrollArea;
        int parentHeight;
        int plotWidth;

        QPainter *painter;

        QPoint getPoint(int x, int y);
        QPoint getSample(int x, int y);
        QPoint lastPos, startPos;

        double gyroCalibrationData[12];
        double calibrationData[12];
};

#endif // GRAPH_H
