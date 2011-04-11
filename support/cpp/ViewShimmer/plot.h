#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class MoteCurve;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget * = NULL);
    virtual ~Plot();

    void start();
    void stop();
    virtual void replot();

public Q_SLOTS:
    void setIntervalLength(double);    
    void showCurve(QwtPlotItem *item, bool on);

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void timerEvent(QTimerEvent *);

private:
    void updateCurve();
    void incrementInterval();

    QwtPlotMarker *d_origin;
    MoteCurve *d_curve;
    int d_paintedPoints;

    QwtPlotDirectPainter *d_directPainter;

    QwtInterval d_interval;
    int d_timerId;

    QwtSystemClock d_clock;
};
