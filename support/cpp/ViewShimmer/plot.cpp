#include "plot.h"
#include "curvedata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>

#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>

#include <qwt_legend.h>
#include <qwt_legend_item.h>

#include <scrollzoomer.h>

#include "Application.h"


const unsigned int c_rangeMax = 10000;

/*class Zoomer: public ScrollZoomer
{
public:
    Zoomer(QwtPlotCanvas *canvas, Application &app):
        ScrollZoomer(canvas),
        application(app)
    {
    }

    virtual void rescale()
    {
        double zoomRatio = (zoomRect().bottomRight().x() - zoomRect().bottomLeft().x()) / zoomBase().width();

        qDebug() << "ZoomRatio: " << zoomRatio;

        //application.window.calculateCurveDatas(zoomRatio);

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


    Application &application;
};*/



class MoteCurve: public QwtPlotCurve
{
public:
    MoteCurve(const QString &title):
        QwtPlotCurve(title)
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;
        c.setAlpha(150);

        setPen(c);
        //setBrush(c);
    }
};


Plot::Plot(QWidget *parent, Application& app):
    application(app),
    QwtPlot(parent),
    //d_paintedPoints(0),
    d_interval(0.0, 100.0)
{
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot(false);
    // We don't need the cache here
    // API Change?
    //canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
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

    setAxisTitle(QwtPlot::xBottom, "Time [s]");


    connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),
        SLOT(showCurve(QwtPlotItem *, bool)));
}

Plot::~Plot()
{
    delete d_directPainter;
}

void Plot::setMoteCurve(int mote)
{
    if(mote ==  -1){
        MoteCurve* curve = new MoteCurve("Online Mote");

        curve->setStyle(QwtPlotCurve::Lines);

        QColor curveColor = QColor(Qt::red);
        curve->setColor(curveColor);
    #if 1
        curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    #endif
    #if 1
        curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
    #endif
        curve->setData(application.window.curve_data_at(0));
        curve->attach(this);

        d_curves.append(curve);

        showCurve(curve, true);
    } else {
        MoteCurve* curve = new MoteCurve("Mote: "+QString::number(application.moteDataHolder.mote(mote)->getMoteID()));

        curve->setStyle(QwtPlotCurve::Lines);

        QColor curveColor = QColor(Qt::red);
        curveColor.setHsv(mote*(359/4),255,255);

        curve->setColor(curveColor);
    #if 1
        curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    #endif
    #if 1
        curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
    #endif
        curve->setData(application.window.curve_data_at(mote));
        curve->attach(this);

        d_curves.append(curve);

        showCurve(curve, true);
    }

}

void Plot::clearCurves()
{
    for(int i=0; i<d_curves.size(); i++){
        d_curves[i]->detach();
        delete d_curves[i];
    }

    d_curves.clear();
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

void Plot::createZoomer()
{
    d_zoomer = new ScrollZoomer(canvas(), application);
    d_zoomer->setRubberBandPen(QColor(Qt::red));
    d_zoomer->setTrackerPen(QColor(Qt::red));

    enableZoomMode(false);
}

void Plot::enableZoomMode(bool on)
{
    if(d_zoomer != NULL){
        d_zoomer->zoomOut();
        d_zoomer->setEnabled(on); // BUG Segmentation fault
    }

}

void Plot::deleteZoomer()
{
    delete d_zoomer;
}
