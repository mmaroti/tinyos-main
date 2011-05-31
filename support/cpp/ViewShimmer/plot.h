#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <QVarLengthArray>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class MoteCurve;
class Application;
class Zoomer;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget *, Application& app);
    virtual ~Plot();

    //virtual void replot();
    void setMoteCurve(int mote);
    void clearCurves();

    void createZoomer();

public Q_SLOTS:
    void showCurve(QwtPlotItem *item, bool on);

    void enableZoomMode(bool);

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

    Zoomer *d_zoomer;

    QwtInterval d_interval;

};
