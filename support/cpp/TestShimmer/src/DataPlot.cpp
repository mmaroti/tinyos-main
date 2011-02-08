/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include <QPainter>
#include <QtDebug>
#include <QPaintEvent>
#include <QScrollArea>
#include "DataPlot.h"
#include "Application.h"
#include "DataRecorder.h"
#include "math.h"
#include "PlotScrollArea.h"
#include "GLWidget.h"
#include "constants.h"

DataPlot::DataPlot(PlotScrollArea *parent, Application &app) : QWidget(parent),
        application(app)
{
        scrollArea = parent;
        graphs = XACC| YACC | ZACC | TIME | GRID;

        //connect(&app.dataRecorder, SIGNAL(sampleAdded()), this, SLOT(onSampleAdded()), Qt::QueuedConnection);
        connect(&app.dataRecorder, SIGNAL(samplesCleared()), this, SLOT(onSamplesCleared()));

        //loadSettingsData();

        resize(QWIDGETSIZE_MAX, 1000);

        lag = 0;
}

QPoint DataPlot::getSample(int x, int y)
{
        if( parentHeight <= 1 )
                parentHeight = 2;

        y = 4095 - (y * 4095 / (parentHeight - 1));
        return QPoint(x, y);
}

QPoint DataPlot::getPoint(int x, int y)
{
        y = (parentHeight - 1) * (4095 - y) / 4095;
        return QPoint(x, y);
}

void DataPlot::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);     //painter = new QPainter(this); //painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        parentHeight = parentWidget()->height();
        QRect rect = event->rect();

        // DEBUGTOOL - to see which area is beeing refreshed
        //painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());

        const DataRecorder &dataRecorder = application.dataRecorder;

        int x0 = rect.left() - 2-lag;
        int x1 = rect.right() + 2 +lag;

        if( x0 < dataRecorder.size() )
        {
            if( x0 < 0 )
                    x0 = 0;
            if( x1 > dataRecorder.size() )
                    x1 = dataRecorder.size();

            //DEBUGTOOL - For showing data lag in system
            /*painter.setPen(QPen(Qt::red, 1, Qt::DashLine));
            for(int i = x0 + 1; i < x1; ++i){
                if( (dataRecorder.at(i).time - dataRecorder.at(i-1).time) > LAG_THRESHOLD ){
                    //x1 += application.dataRecorder.getTime(i);
                    painter.drawLine(getPoint(application.dataRecorder.getTime(i-1), 0), getPoint(application.dataRecorder.getTime(i), 4096));
                }
            }*/

            //for showing selection in the plot
            if( (startPos.x() != 0) && lastPos.x() != 0 ){
                painter.setPen(QPen(Qt::yellow, 1, Qt::SolidLine));
                for(int i = x0 + 1; i < x1; ++i){
                    painter.drawLine(getPoint(startPos.x(), 0), getPoint(startPos.x(), 4096));
                    painter.drawLine(getPoint(lastPos.x(), 0), getPoint(lastPos.x(), 4096));
                    //painter.drawLine(getPoint(startPos.x(), 0), getPoint(lastPos.x(), 4096));
                }
            }

            if( (graphs & XRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xAccel), getPoint(i, dataRecorder.at(i).xAccel));
            }

            if( (graphs & YRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yAccel), getPoint(i, dataRecorder.at(i).yAccel));
            }

            if( (graphs & ZRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).zAccel), getPoint(i, dataRecorder.at(i).zAccel));
            }

            if( (graphs & XRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xGyro), getPoint(i, dataRecorder.at(i).xGyro));
            }

            if( (graphs & YRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yGyro), getPoint(i, dataRecorder.at(i).yGyro));
            }

            if( (graphs & ZRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::magenta, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).zGyro), getPoint(i, dataRecorder.at(i).zGyro));
            }

            if( (graphs & VOLTAGE) != 0 )
            {
                    painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).voltage), getPoint(i, dataRecorder.at(i).voltage));
            }

            if( (graphs & TEMP) != 0 )
            {
                    painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).temp), getPoint(i, dataRecorder.at(i).temp));
            }

            if( (graphs & XACC) != 0 )
            {
                painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("xAcc", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * 512/GRAV + 2048), getPoint(i, xtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & YACC) != 0 )
            {
                painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
                double ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = application.dataRecorder.calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = application.dataRecorder.calculateCalibratedValue("yAcc", i);
                    painter.drawLine(getPoint(i-1, ytemp1 * 512/GRAV + 2048), getPoint(i, ytemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & ZACC) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                double ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ztemp1 = application.dataRecorder.calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = application.dataRecorder.calculateCalibratedValue("zAcc", i);
                    painter.drawLine(getPoint(i-1, ztemp1 * 512/GRAV + 2048), getPoint(i, ztemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & ABSACC) != 0 )
            {
                painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));
                double avgtemp1, avgtemp2;                
                for(int i = x0 + 1; i < x1; ++i) {
                    avgtemp2 = application.dataRecorder.calculateAbsAcc(i);
                    avgtemp1 = application.dataRecorder.calculateAbsAcc(i-1);

                    painter.drawLine(getPoint(i-1, avgtemp1 * 512/GRAV + 2048), getPoint(i, avgtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & XYANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("xAcc", i);
                    ytemp1 = application.dataRecorder.calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = application.dataRecorder.calculateCalibratedValue("yAcc", i);

                    //alfa1 = application.dataRecorder.calculateAngle(xtemp1, ytemp1);
                    //alfa2 = application.dataRecorder.calculateAngle(xtemp2, ytemp2);

                    alfa1 = atan2(-xtemp1, -ytemp1);
                    alfa2 = atan2(-xtemp2, -ytemp2);

                    //emit angleChanged(alfa1);
                    //HACK
                    application.dataRecorder.setAngle(i).XYangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & YZANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkGreen, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double ytemp1, ytemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = application.dataRecorder.calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = application.dataRecorder.calculateCalibratedValue("yAcc", i);
                    ztemp1 = application.dataRecorder.calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = application.dataRecorder.calculateCalibratedValue("zAcc", i);

                    //alfa1 = application.dataRecorder.calculateAngle(ztemp1, ytemp1);
                    //alfa2 = application.dataRecorder.calculateAngle(ztemp2, ytemp2);

                    alfa1 = atan2(-ytemp1, -ztemp1);
                    alfa2 = atan2(-ytemp2, -ztemp2);
                    //HACK
                    application.dataRecorder.setAngle(i).YZangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & ZXANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkRed, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("xAcc", i);
                    ztemp1 = application.dataRecorder.calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = application.dataRecorder.calculateCalibratedValue("zAcc", i);

                    //alfa1 = application.dataRecorder.calculateAngle(ztemp1, xtemp1);
                    //alfa2 = application.dataRecorder.calculateAngle(ztemp2, xtemp2);

                    alfa1 = atan2(-xtemp1, -ztemp1);
                    alfa2 = atan2(-xtemp2, -ztemp2);

                    //HACK
                    application.dataRecorder.setAngle(i).ZXangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & XGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("xGyro", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("xGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & YGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("yGyro", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("yGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & ZGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::magenta, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("zGyro", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("zGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & XANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double xtemp1, xtemp2, alfa1, alfa2, avg1, avg2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = application.dataRecorder.calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = application.dataRecorder.calculateCalibratedValue("xAcc", i);

                    avg1 = application.dataRecorder.calculateAbsAcc(i-1);
                    avg2 = application.dataRecorder.calculateAbsAcc(i);

                    double param1, param2;
                    param1 = xtemp1 / avg1;
                    param2 = xtemp2 / avg2;
                    //if(param1 > 1) param1 = 1; if(param1 < -1) param1 = -1;
                    //if(param2 > 1) param2 = 1; if(param2 < -1) param2 = -1;
                    alfa1 = asin(param1);
                    alfa2 = asin(param2);

                    if(fabs(alfa1 -alfa2) < (ANGLEMAX - ANGLEMIN)/2){
                        painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                    } else if(alfa1>alfa2){
                        painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, ANGLEMAX * 2048/M_PI + 2048));
                    } else {
                        painter.drawLine(getPoint(i-1, ANGLEMIN * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                    }

                }
            }

            if( (graphs & YANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double ytemp1, ytemp2, alfa1, alfa2, avg1, avg2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = application.dataRecorder.calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = application.dataRecorder.calculateCalibratedValue("yAcc", i);

                    avg1 = application.dataRecorder.calculateAbsAcc(i-1);
                    avg2 = application.dataRecorder.calculateAbsAcc(i);

                    double param1, param2;
                    param1 = ytemp1 / avg1;
                    param2 = ytemp2 / avg2;
                    if(param1 > 1) param1 = 1; if(param1 < -1) param1 = -1;
                    if(param2 > 1) param2 = 1; if(param2 < -1) param2 = -1;
                    alfa1 = asin(param1);
                    alfa2 = asin(param2);
                    painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                }
            }

            if( (graphs & ZANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double ztemp1, ztemp2, alfa1, alfa2, avg1, avg2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ztemp1 = application.dataRecorder.calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = application.dataRecorder.calculateCalibratedValue("zAcc", i);

                    avg1 = application.dataRecorder.calculateAbsAcc(i-1);
                    avg2 = application.dataRecorder.calculateAbsAcc(i);

                    double param1, param2;
                    param1 = ztemp1 / avg1;
                    param2 = ztemp2 / avg2;
                    if(param1 > 1) param1 = 1; if(param1 < -1) param1 = -1;
                    if(param2 > 1) param2 = 1; if(param2 < -1) param2 = -1;
                    alfa1 = asin(param1);
                    alfa2 = asin(param2);
                    painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                }
            }

            if( (graphs & XEUL) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));

                double angle1_deg = 0;
                double angle2_deg = 0;

                for(int i = x0 + 1; i < x1; ++i) {
                    dataRecorder.euler_angle(i-1, X, angle1_deg);
                    dataRecorder.euler_angle(i  , X, angle2_deg);
                    painter.drawLine(getPoint(i-1, angle1_deg * (2048/M_PI) + 2048), getPoint(i, angle2_deg * (2048/M_PI) + 2048));
                }
            }

            if( (graphs & YEUL) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));

                double angle1_deg = 0;
                double angle2_deg = 0;

                for(int i = x0 + 1; i < x1; ++i) {
                    dataRecorder.euler_angle(i-1, Y, angle1_deg);
                    dataRecorder.euler_angle(i  , Y, angle2_deg);
                    painter.drawLine(getPoint(i-1, angle1_deg * (2048/M_PI) + 2048), getPoint(i, angle2_deg * (2048/M_PI) + 2048));
                }
            }
/*
            if( (graphs & ZEUL) != 0 )
            {
                painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));

                double angle1_deg = 0;
                double angle2_deg = 0;

                for(int i = x0 + 1; i < x1; ++i) {
                    dataRecorder.euler_angle(i-1, Z, angle1_deg);
                    dataRecorder.euler_angle(i  , Z, angle2_deg);
                    painter.drawLine(getPoint(i-1, angle1_deg * (2048/M_PI) + 2048), getPoint(i, angle2_deg * (2048/M_PI) + 2048));
                }
            }
*/
            if( (graphs & XINT) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.integrated_angle(i, X);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }

            if( (graphs & YINT) != 0 )
            {
                painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.integrated_angle(i, Y);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }

            if( (graphs & ZINT) != 0 )
            {
                painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.integrated_angle(i, Z);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }

            if( (graphs & XCORRANG) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.corrected_angle(i, X);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }

            if( (graphs & YCORRANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.corrected_angle(i, Y);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }

            if( (graphs & ZCORRANG) != 0 )
            {
                painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));

                for(int i = x0 + 1; i < x1; ++i) {
                    Angle_pair a = dataRecorder.corrected_angle(i, Z);
                    painter.drawLine(getPoint(i-1, a.angle1*(2048/M_PI)+2048), getPoint(i, a.angle2*(2048/M_PI)+2048));
                }
            }


            if( (graphs & CALIB) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::DashLine));

                for(int i=0; i < 6; i++){
                    if(application.dataRecorder.getAccelIdleWindowStart()[i] != -1){
                        painter.drawLine(getPoint(application.dataRecorder.getAccelIdleWindowStart()[i], 0), getPoint(application.dataRecorder.getAccelIdleWindowStart()[i], 4000));
                        painter.drawLine(getPoint(application.dataRecorder.getAccelIdleWindowStart()[i]+WINDOW, 0), getPoint(application.dataRecorder.getAccelIdleWindowStart()[i]+WINDOW, 4000));
                    }
                }

                painter.setPen(QPen(Qt::green, 2, Qt::DashLine));

                for(int i=0; i < 7; i++){
                    if(application.dataRecorder.getGyroIdleWindowStart()[i] != -1){
                        painter.drawLine(getPoint(application.dataRecorder.getGyroIdleWindowStart()[i], 0), getPoint(application.dataRecorder.getGyroIdleWindowStart()[i], 4000));
                        painter.drawLine(getPoint(application.dataRecorder.getGyroIdleWindowStart()[i]+GYROWINDOW, 0), getPoint(application.dataRecorder.getGyroIdleWindowStart()[i]+GYROWINDOW, 4000));
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

            double HZ = C_HZ;
            double TICKS = C_TICKS;

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

void DataPlot::setGraphs(int graphs, bool on)
{
        if( on )
                this->graphs |= graphs;
        else
                this->graphs &= ~graphs;

        QWidget::update();
}

void DataPlot::onSampleAdded()
{
        if( plotWidth != application.dataRecorder.size() )
        {
                int oldWidth = plotWidth;
                plotWidth = application.dataRecorder.size();
                /*if(application.dataRecorder.getTime(application.dataRecorder.size()-1) != (application.dataRecorder.size()-1)){
                    //lag = (application.dataRecorder.getTime(application.dataRecorder.size()-1)-application.dataRecorder.size());
                    //plotWidth += lag;
                    //scrollArea->horizontalScrollBar()->setValue();
                    //QWidget::update();
                }*/

                scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
                scrollArea->ensureVisible(plotWidth,0,1,1);

                QWidget::update(oldWidth-2, 0, plotWidth-oldWidth+2, parentWidget()->height());
        }
}

void DataPlot::onSamplesCleared()
{
        plotWidth = application.dataRecorder.size();
        scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
        scrollArea->ensureVisible(plotWidth,0,1,1);
        QWidget::update(0, 0, parentWidget()->width(), parentWidget()->height());
}

void DataPlot::onNewCalibration()
{
    //loadSettingsData();

    QWidget::update();
}

void DataPlot::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton) {
        startPos = event->pos();
        QPoint sample = getSample(event->pos().x(), event->pos().y());
        QString message = "Time: " + QString::number(sample.x()/C_HZ, 'f', 1) + " sec  ";
        message.append("("+QString::number(sample.x())+". sample - ");
        message.append(" "+QString::number(application.dataRecorder.getTime(sample.x()))+" mote time.) ");
        if( (graphs & XRAWACC) != 0 || (graphs & YRAWACC) != 0 || (graphs & ZRAWACC) != 0 || (graphs & XRAWGYRO) != 0 || (graphs & YRAWGYRO) != 0 || (graphs & ZRAWGYRO) != 0 ){
            message.append(" Value: " + QString::number(sample.y()) + " ");
        }
        if( (graphs & XACC) != 0 || (graphs & YACC) != 0 || (graphs & ZACC) != 0 || (graphs & ABSACC) != 0 ){
            message.append(" , Acceleration: " + QString::number((double)((sample.y()-2048)/(512/GRAV)), 'f', 2) + " m/s^2");
        }
        if( (graphs & XYANG) != 0 || (graphs & YZANG) != 0 || (graphs & ZXANG) != 0 || (graphs & XANG) != 0 || (graphs & YANG) != 0 || (graphs & ZANG) != 0 || (graphs & XEUL) != 0 || (graphs & YEUL) != 0 || (graphs & ZEUL) != 0 || (graphs & XINT) != 0 || (graphs & YINT) != 0 || (graphs & ZINT) != 0 || (graphs & XCORRANG) != 0 || (graphs & YCORRANG) != 0 || (graphs & ZCORRANG) != 0){
            message.append(",  Angle: " + QString::number((sample.y()-2048)/(2048/M_PI), 'f', 1) + "rad; "  + QString::number( (sample.y()-2048)/(2048/M_PI)*57.296, 'f', 2 ) + QString::fromUtf8("°."));
        }
        if( (graphs & XGYRO) != 0 || (graphs & YGYRO) != 0 || (graphs & ZGYRO) != 0 ){
            message.append(",  Gyroscope: " + QString::number((sample.y()-2048)/(2048/(4*M_PI)),'f',1) + "rad/sec; " + QString::number( ((sample.y()-2048)/(2048/(4*M_PI)))*RADIAN, 'f', 2 ) + QString::fromUtf8("°/sec; ") + QString::number( (((sample.y()-2048)/(2048/(4*M_PI)))*60)/(2*M_PI), 'f', 1 ) + "rpm." );
        }
        if( !lastPos.isNull() ){
            emit calculateRange(startPos.x(),lastPos.x());
        }
        application.showMessage( message );
    } else if (event->buttons() & Qt::RightButton) {
        //painter->backgroundMode(Qt::TransparentMode);
        lastPos = event->pos();
        if( !startPos.isNull() ){
            emit calculateRange(startPos.x(),lastPos.x());
        }
    }

    if(lastPos.x()<startPos.x()){
        application.dataRecorder.from = lastPos.x();
        application.dataRecorder.to = startPos.x();
    }else{
        application.dataRecorder.to = lastPos.x();
        application.dataRecorder.from = startPos.x();
    }
    QWidget::update();

}

void DataPlot::mouseMoveEvent(QMouseEvent *event)
{
    //int dx = event->x() - lastPos.x();
    //int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {

    } else if (event->buttons() & Qt::RightButton) {

    }
    lastPos = event->pos();
}

void DataPlot::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}
