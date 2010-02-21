
#include <QPainter>
#include <QtDebug>
#include <QPaintEvent>
#include <QScrollArea>
#include "RawDataPlot.h"
#include "Application.h"
#include "DataRecorder.h"
#include "math.h"
#include "PlotScrollArea.h"

RawDataPlot::RawDataPlot(PlotScrollArea *parent, Application &app) : QWidget(parent),
	dataRecorder(app.dataRecorder)
{
	scrollArea = parent;
	graphs = XACCEL | YACCEL | ZACCEL | XGYRO | YGYRO | ZGYRO | GRID | TIME;

	connect(&app.dataRecorder, SIGNAL(sampleAdded()), this, SLOT(onSampleAdded()));
	connect(&app.dataRecorder, SIGNAL(samplesCleared()), this, SLOT(onSamplesCleared()));

	resize(QWIDGETSIZE_MAX, 1000);
}

QPoint RawDataPlot::getPoint(int x, int y)
{
	y = (parentHeight - 1) * (4095 - y) / 4095;
	return QPoint(x, y);
}

void RawDataPlot::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	parentHeight = parentWidget()->height();
	QRect rect = event->rect();

	if( (graphs & GRID) != 0 )
	{
		painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
		painter.drawLine(0, parentHeight/2, width(), parentHeight/2);
		painter.drawLine(0, parentHeight/4, width(), parentHeight/4);
		painter.drawLine(0, 3*parentHeight/4, width(), 3*parentHeight/4);
	}

	if( (graphs & TIME) != 0 )
	{
		painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));

		double HZ = 204.8;
		double TICKS = 2;

		int start = floor((rect.left() - 20) * TICKS / HZ);
		int end = ceil((rect.right() + 20) * TICKS / HZ);

		for(int i = start; i <= end; ++i)
		{
			QPoint xp = getPoint(i * HZ / TICKS, 2048);
			painter.drawLine(xp.x(), xp.y() - 5, xp.x(), xp.y() + 5);

			double sec = (double)i / TICKS;

			painter.drawText(xp.x() - 20, xp.y() - 15, 41, 10, Qt::AlignCenter, QString::number(sec, 'f', 1));
		}
	}

	// to see which area is beeing refreshed
//	painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());

	int x0 = rect.left() - 2;
	int x1 = rect.right() + 2;

	if( x0 >= dataRecorder.size() )
		return;

	if( x0 < 0 )
		x0 = 0;
	if( x1 > dataRecorder.size() )
		x1 = dataRecorder.size();

	if( (graphs & XACCEL) != 0 )
	{
		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		for(int i = x0 + 1; i < x1; ++i)
			painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xAccel), getPoint(i, dataRecorder.at(i).xAccel));
	}

	if( (graphs & YACCEL) != 0 )
	{
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		for(int i = x0 + 1; i < x1; ++i)
			painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yAccel), getPoint(i, dataRecorder.at(i).yAccel));
	}

	if( (graphs & ZACCEL) != 0 )
	{
		painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
		for(int i = x0 + 1; i < x1; ++i)
			painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).zAccel), getPoint(i, dataRecorder.at(i).zAccel));
	}

	if( (graphs & XGYRO) != 0 )
	{
		painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
		for(int i = x0 + 1; i < x1; ++i)
			painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xGyro), getPoint(i, dataRecorder.at(i).xGyro));
	}

	if( (graphs & YGYRO) != 0 )
	{
		painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
		for(int i = x0 + 1; i < x1; ++i)
			painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yGyro), getPoint(i, dataRecorder.at(i).yGyro));
	}

	if( (graphs & ZGYRO) != 0 )
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
}

void RawDataPlot::onSampleAdded()
{
	if( plotWidth != dataRecorder.size() )
	{
		int oldWidth = plotWidth;
		plotWidth = dataRecorder.size();

		scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
		scrollArea->ensureVisible(plotWidth,0,1,1);

		QWidget::update(oldWidth-2, 0, plotWidth-oldWidth+2, parentWidget()->height());
	}
}

void RawDataPlot::onSamplesCleared()
{
	plotWidth = dataRecorder.size();
	scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
	scrollArea->ensureVisible(plotWidth,0,1,1);
	QWidget::update(0, 0, parentWidget()->width(), parentWidget()->height());
}

QSize RawDataPlot::size() const
{
	return QSize(dataRecorder.size(), 0);
}

void RawDataPlot::setGraphs(int graphs, bool on)
{
	if( on )
		this->graphs |= graphs;
	else
		this->graphs &= ~graphs;

	QWidget::update();
}
