#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <QVarLengthArray>

class QwtPlotMarker;
class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class MoteCurve;
class Application;
class ScrollZoomer;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget *, Application& app);
    virtual ~Plot();

    //virtual void replot();
    void addMoteCurve(int mote);
    void clearCurves();

    void createZoomer();
    void enableZoomMode(bool);
    void deleteZoomer();
    void zoom();
    void updateScroll();

public Q_SLOTS:
    void showCurve(QwtPlotItem *item, bool on);

protected:
    virtual void resizeEvent(QResizeEvent *);
    //virtual void timerEvent(QTimerEvent *);

private:
    Application& application;
    void updateCurve();
    //void incrementInterval();

    QwtPlotMarker *d_origin;
    QVarLengthArray<MoteCurve*> d_curves;
    //int d_paintedPoints;

    QwtPlotDirectPainter *d_directPainter;

    ScrollZoomer *d_zoomer;
    QwtPlotMarker *d_marker1;

    QwtInterval d_interval;

};
