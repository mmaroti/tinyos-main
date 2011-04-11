#include "plot.h"
#include "curvedata.h"
#include "DataRecorder.h"
#include "signaldata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>
#include "scrollzoomer.h"

#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>

#include <qwt_legend.h>
#include <qwt_legend_item.h>


const unsigned int c_rangeMax = 1000;

class Zoomer: public ScrollZoomer
{
public:
    Zoomer(QwtPlotCanvas *canvas):
        ScrollZoomer(canvas)
    {
    }

    virtual void rescale()
    {
        QwtScaleWidget *scaleWidget = plot()->axisWidget(yAxis());
        QwtScaleDraw *sd = scaleWidget->scaleDraw();

        int minExtent = 0;
        if ( zoomRectIndex() > 0 )
        {
            // When scrolling in vertical direction
            // the plot is jumping in horizontal direction
            // because of the different widths of the labels
            // So we better use a fixed extent.

            minExtent = sd->spacing() + sd->majTickLength() + 1;
            minExtent += sd->labelSize(scaleWidget->font(), c_rangeMax).width();
        }

        sd->setMinimumExtent(minExtent);

        ScrollZoomer::rescale();
    }
};

class MoteCurve: public QwtPlotCurve
{
public:
    MoteCurve(const QString &title):
        QwtPlotCurve(title)
    {
        //setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;
        c.setAlpha(150);

        setPen(c);
        //setBrush(c);
    }
};


Plot::Plot(QWidget *parent):
    QwtPlot(parent),
    d_paintedPoints(0),
    d_interval(0.0, 10.0),
    d_timerId(-1)
{
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot(false);

    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);


#if defined(Q_WS_X11)
    // Even if not recommended by TrollTech, Qt::WA_PaintOutsidePaintEvent
    // works on X11. This has a nice effect on the performance.
    
    canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif

    plotLayout()->setAlignCanvasToScales(true);

    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    insertLegend(legend, QwtPlot::RightLegend);

    setAxisTitle(QwtPlot::xBottom, "Time [s]");
    setAxisScale(QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue()); 
    setAxisScale(QwtPlot::yLeft, -180.0, 180.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(Qt::gray, 0.0, Qt::DotLine));
    grid->enableX(true);
    grid->enableXMin(true);
    grid->enableY(true);
    grid->enableYMin(false);
    grid->attach(this);

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle(QwtPlotMarker::Cross);
    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::gray, 0.0, Qt::DashLine));
    d_origin->attach(this);

    d_curve = new MoteCurve("Mote 1");
    d_curve->setStyle(QwtPlotCurve::Lines);
    d_curve->setColor(Qt::red);
#if 1
    d_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
#if 1
    d_curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
#endif
    d_curve->setData(new CurveData());
    d_curve->attach(this);

    // enable zooming

    Zoomer *zoomer = new Zoomer(canvas());
    zoomer->setRubberBandPen(QPen(Qt::red, 2, Qt::DotLine));
    zoomer->setTrackerPen(QPen(Qt::red));

    connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),
        SLOT(showCurve(QwtPlotItem *, bool)));
}

Plot::~Plot()
{
    delete d_directPainter;
}

void Plot::start()
{
    d_clock.start();
    d_timerId = startTimer(10);
}

void Plot::stop()
{
   killTimer(d_timerId);
}

void Plot::replot()
{
    CurveData *data = (CurveData *)d_curve->data();
    data->values().lock();

    QwtPlot::replot();
    d_paintedPoints = data->size();

    data->values().unlock();
}

void Plot::setIntervalLength(double interval)
{
    if ( interval > 0.0 && interval != d_interval.width() )
    {
        d_interval.setMaxValue(d_interval.minValue() + interval);
        setAxisScale(QwtPlot::xBottom, 
            d_interval.minValue(), d_interval.maxValue());

        replot();
    }
}

void Plot::updateCurve()
{
    CurveData *data = (CurveData *)d_curve->data();
    data->values().lock();

    const int numPoints = d_curve->data()->size();
    if ( numPoints > d_paintedPoints )
    {
        d_directPainter->drawSeries(d_curve, 
            d_paintedPoints - 1, numPoints - 1);
        d_paintedPoints = numPoints;
    }

    data->values().unlock();
}

void Plot::incrementInterval()
{
    d_interval = QwtInterval(d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width());

    CurveData *data = (CurveData *)d_curve->data();
    data->values().value(data->values().size()-1).x();
    data->values().clearStaleValues(d_interval.minValue());

    // To avoid, that the grid is jumping, we disable 
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = *axisScaleDiv(QwtPlot::xBottom);
    scaleDiv.setInterval(d_interval);

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks(i);
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks(i, ticks);
    }
    setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);

    d_paintedPoints = 0;
    replot();
}

void Plot::timerEvent(QTimerEvent *event)
{

    if ( event->timerId() == d_timerId )
    {
        updateCurve();

        double elapsed = d_clock.elapsed() / 1000.0;
        qDebug() << elapsed;
        CurveData *data = (CurveData *)d_curve->data();

        if(data->values().size() > 0) elapsed = data->values().value(data->values().size()-1).x();
        if ( elapsed > d_interval.maxValue() )
            incrementInterval();

        return;
    }

    QwtPlot::timerEvent(event);
}

void Plot::resizeEvent(QResizeEvent *event)
{
    d_directPainter->reset();
    QwtPlot::resizeEvent(event);

    const QColor color(46, 74, 95);
    const QRect cr = canvas()->contentsRect();
    QLinearGradient gradient(cr.topLeft(), cr.topRight());
    gradient.setColorAt(0.0, color.light(130));
    gradient.setColorAt(0.2, color.dark(110));
    gradient.setColorAt(0.7, color);
    gradient.setColorAt(1.0, color.dark(150));

    QPalette pal = canvas()->palette();
    pal.setBrush(QPalette::Window, QBrush(gradient));
    canvas()->setPalette(pal);
}

void Plot::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);

    replot();
}
