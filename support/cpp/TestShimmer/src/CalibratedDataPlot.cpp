#include <QPainter>
#include <QtDebug>
#include <QPaintEvent>
#include <QScrollArea>
#include "CalibratedDataPlot.h"
#include "Application.h"
#include "DataRecorder.h"
#include "math.h"
#include "PlotScrollArea.h"
#include "CalibrationWidget.h"
#include "CalibrationModule.h"

CalibratedDataPlot::CalibratedDataPlot(PlotScrollArea *parent, Application &app) : QWidget(parent),
        application(app)
{
        scrollArea = parent;
        graphs = XACCEL | YACCEL | ZACCEL | TIME | GRID | AVGACC | XYANGLE | YZANGLE | ZXANGLE;

        connect(&app.dataRecorder, SIGNAL(sampleAdded()), this, SLOT(onSampleAdded()));
        connect(&app.dataRecorder, SIGNAL(samplesCleared()), this, SLOT(onSamplesCleared()));

        int size = application.settings.beginReadArray("calibrationData");
        for (int i = 0; i < size; ++i) {
            application.settings.setArrayIndex(i);

            calibrationData[i] = application.settings.value("calibrationData").toDouble();
        }
        application.settings.endArray();


        resize(QWIDGETSIZE_MAX, 1000);
}

QPoint CalibratedDataPlot::getSample(int x, int y)
{
        if( parentHeight <= 1 )
                parentHeight = 2;

        y = 4095 - (y * 4095 / (parentHeight - 1));
        return QPoint(x, y);
}

QPoint CalibratedDataPlot::getPoint(int x, int y)
{
        y = (parentHeight - 1) * (4095 - y) / 4095;
        return QPoint(x, y);
}

void CalibratedDataPlot::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        parentHeight = parentWidget()->height();
        QRect rect = event->rect();

        // to see which area is beeing refreshed
        //	painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());

        const DataRecorder &dataRecorder = application.dataRecorder;

        int x0 = rect.left() - 2;
        int x1 = rect.right() + 2;

        if( x0 < application.dataRecorder.size() )
        {
            if( x0 < 0 )
                    x0 = 0;
            if( x1 > application.dataRecorder.size() )
                    x1 = dataRecorder.size();

            if( (graphs & XACCEL) != 0 )
            {
                painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[0] + application.dataRecorder.at(i-1).yAccel * calibrationData[1] + application.dataRecorder.at(i-1).zAccel * calibrationData[2] + calibrationData[9]);
                    xtemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[0] + application.dataRecorder.at(i).yAccel * calibrationData[1] + application.dataRecorder.at(i).zAccel * calibrationData[2] + calibrationData[9]);
                    painter.drawLine(getPoint(i-1, xtemp1 * 512/GRAV + 2048), getPoint(i, xtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & YACCEL) != 0 )
            {
                painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
                double ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[3] + application.dataRecorder.at(i-1).yAccel * calibrationData[4] + application.dataRecorder.at(i-1).zAccel * calibrationData[5] + calibrationData[10]);
                    ytemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[3] + application.dataRecorder.at(i).yAccel * calibrationData[4] + application.dataRecorder.at(i).zAccel * calibrationData[5] + calibrationData[10]);
                    painter.drawLine(getPoint(i-1, ytemp1 * 512/GRAV + 2048), getPoint(i, ytemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & ZACCEL) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                double ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ztemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[6] + application.dataRecorder.at(i-1).yAccel * calibrationData[7] + application.dataRecorder.at(i-1).zAccel * calibrationData[8] + calibrationData[11]);
                    ztemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[6] + application.dataRecorder.at(i).yAccel * calibrationData[7] + application.dataRecorder.at(i).zAccel * calibrationData[8] + calibrationData[11]);
                    painter.drawLine(getPoint(i-1, ztemp1 * 512/GRAV + 2048), getPoint(i, ztemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & AVGACC) != 0 )
            {
                painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                double avgtemp1, avgtemp2;
                double xtemp1, xtemp2, ytemp1, ytemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[0] + application.dataRecorder.at(i-1).yAccel * calibrationData[1] + application.dataRecorder.at(i-1).zAccel * calibrationData[2] + calibrationData[9]);
                    xtemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[0] + application.dataRecorder.at(i).yAccel * calibrationData[1] + application.dataRecorder.at(i).zAccel * calibrationData[2] + calibrationData[9]);
                    ytemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[3] + application.dataRecorder.at(i-1).yAccel * calibrationData[4] + application.dataRecorder.at(i-1).zAccel * calibrationData[5] + calibrationData[10]);
                    ytemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[3] + application.dataRecorder.at(i).yAccel * calibrationData[4] + application.dataRecorder.at(i).zAccel * calibrationData[5] + calibrationData[10]);
                    ztemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[6] + application.dataRecorder.at(i-1).yAccel * calibrationData[7] + application.dataRecorder.at(i-1).zAccel * calibrationData[8] + calibrationData[11]);
                    ztemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[6] + application.dataRecorder.at(i).yAccel * calibrationData[7] + application.dataRecorder.at(i).zAccel * calibrationData[8] + calibrationData[11]);

                    avgtemp1 = sqrt( pow(xtemp1, 2.0) + pow(ytemp1, 2.0) + pow(ztemp1, 2.0) );
                    avgtemp2 = sqrt( pow(xtemp2, 2.0) + pow(ytemp2, 2.0) + pow(ztemp2, 2.0) );
                    painter.drawLine(getPoint(i-1, avgtemp1 * 512/GRAV + 2048), getPoint(i, avgtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & XYANGLE) != 0 )
            {
                painter.setPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[0] + application.dataRecorder.at(i-1).yAccel * calibrationData[1] + application.dataRecorder.at(i-1).zAccel * calibrationData[2] + calibrationData[9]);
                    xtemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[0] + application.dataRecorder.at(i).yAccel * calibrationData[1] + application.dataRecorder.at(i).zAccel * calibrationData[2] + calibrationData[9]);
                    ytemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[3] + application.dataRecorder.at(i-1).yAccel * calibrationData[4] + application.dataRecorder.at(i-1).zAccel * calibrationData[5] + calibrationData[10]);
                    ytemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[3] + application.dataRecorder.at(i).yAccel * calibrationData[4] + application.dataRecorder.at(i).zAccel * calibrationData[5] + calibrationData[10]);

                    alfa1 = calculateAngle(xtemp1, ytemp1);
                    alfa2 = calculateAngle(xtemp2, ytemp2);

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & YZANGLE) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double ytemp1, ytemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[3] + application.dataRecorder.at(i-1).yAccel * calibrationData[4] + application.dataRecorder.at(i-1).zAccel * calibrationData[5] + calibrationData[10]);
                    ytemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[3] + application.dataRecorder.at(i).yAccel * calibrationData[4] + application.dataRecorder.at(i).zAccel * calibrationData[5] + calibrationData[10]);
                    ztemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[6] + application.dataRecorder.at(i-1).yAccel * calibrationData[7] + application.dataRecorder.at(i-1).zAccel * calibrationData[8] + calibrationData[11]);
                    ztemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[6] + application.dataRecorder.at(i).yAccel * calibrationData[7] + application.dataRecorder.at(i).zAccel * calibrationData[8] + calibrationData[11]);

                    alfa1 = calculateAngle(ytemp1, ztemp1);
                    alfa2 = calculateAngle(ytemp2, ztemp2);

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & ZXANGLE) != 0 )
            {
                painter.setPen(QPen(Qt::darkRed, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[0] + application.dataRecorder.at(i-1).yAccel * calibrationData[1] + application.dataRecorder.at(i-1).zAccel * calibrationData[2] + calibrationData[9]);
                    xtemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[0] + application.dataRecorder.at(i).yAccel * calibrationData[1] + application.dataRecorder.at(i).zAccel * calibrationData[2] + calibrationData[9]);
                    ztemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationData[6] + application.dataRecorder.at(i-1).yAccel * calibrationData[7] + application.dataRecorder.at(i-1).zAccel * calibrationData[8] + calibrationData[11]);
                    ztemp2 = (application.dataRecorder.at(i).xAccel * calibrationData[6] + application.dataRecorder.at(i).yAccel * calibrationData[7] + application.dataRecorder.at(i).zAccel * calibrationData[8] + calibrationData[11]);

                    alfa1 = calculateAngle(ztemp1, xtemp1);
                    alfa2 = calculateAngle(ztemp2, xtemp2);

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }
        }

        if( (graphs & GRID) != 0 )
        {
            painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
            painter.drawLine(0, parentHeight/2, width(), parentHeight/2);
            painter.drawLine(0, parentHeight/4, width(), parentHeight/4);
            painter.drawLine(0, 3*parentHeight/4, width(), 3*parentHeight/4);
            painter.drawLine(0, parentHeight/8, width(), parentHeight/8);
            painter.drawLine(0, 3*parentHeight/8, width(), 3*parentHeight/8);
            painter.drawLine(0, 5*parentHeight/8, width(), 5*parentHeight/8);
            painter.drawLine(0, 7*parentHeight/8, width(), 7*parentHeight/8);
        }

        if( (graphs & TIME) != 0 ) {
            painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));

            double HZ = 204.8;
            double TICKS = 2;

            int start = floor((rect.left() - 20) * TICKS / HZ);
            int end = ceil((rect.right() + 20) * TICKS / HZ);

            for(int i = start; i <= end; ++i) {
                    QPoint xp = getPoint(i * HZ / TICKS, 2048);
                    painter.drawLine(xp.x(), xp.y() - 5, xp.x(), xp.y() + 5);

                    double sec = (double)i / TICKS;

                    painter.drawText(xp.x() - 20, xp.y() - 15, 41, 10, Qt::AlignCenter, QString::number(sec, 'f', 1));
            }
        }

}

double CalibratedDataPlot::calculateAngle(double accel1, double accel2) {
    double alfa;
    if( (pow(accel1, 2.0) + pow(accel2, 2.0)) < pow(GRAV/2, 2.0) )
        return 10.0;

    if( fabs(accel1) > fabs(accel2) ) {
        alfa = atan(accel2/accel1);
        if( accel1 < 0 )
            alfa += M_PI;
    }
    else {
        alfa = M_PI/2 - atan(accel1/accel2);
        if( accel2 < 0 )
            alfa += M_PI;
    }

    if ( alfa >= M_PI) alfa -= 2*M_PI;
    if ( alfa < -M_PI) alfa += 2*M_PI;

    return alfa;
}

void CalibratedDataPlot::onSampleAdded()
{
        if( plotWidth != application.dataRecorder.size() )
        {
                int oldWidth = plotWidth;
                plotWidth = application.dataRecorder.size();

                scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
                scrollArea->ensureVisible(plotWidth,0,1,1);

                QWidget::update(oldWidth-2, 0, plotWidth-oldWidth+2, parentWidget()->height());
        }
}

void CalibratedDataPlot::onSamplesCleared()
{
        plotWidth = application.dataRecorder.size();
        scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
        scrollArea->ensureVisible(plotWidth,0,1,1);
        QWidget::update(0, 0, parentWidget()->width(), parentWidget()->height());
}

void CalibratedDataPlot::setGraphs(int graphs, bool on)
{
        if( on )
                this->graphs |= graphs;
        else
                this->graphs &= ~graphs;

        QWidget::update();
}

void CalibratedDataPlot::mousePressEvent(QMouseEvent * event)
{
        QPoint sample = getSample(event->pos().x(), event->pos().y());
        application.showMessage("Sample: " + QString::number(sample.y()) + "   Time: " + QString::number(sample.x()/204.8, 'f', 1) + " sec  " + " , Acceleration: " + QString::number((double)((sample.y()-2048)/(512/GRAV)), 'f', 2) + " m/s^2"
                                + "  Angle: " + QString::number((sample.y()-2048)/(2048/M_PI), 'f', 1) + "rad; " + QString::number( (sample.y()-2048)/(2048/M_PI)*57.296, 'f', 2 ) + "°."  );
}
