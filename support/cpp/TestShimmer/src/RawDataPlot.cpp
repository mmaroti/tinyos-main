
#include <QPainter>
#include <QtDebug>
#include <QPaintEvent>
#include <QScrollArea>
#include "RawDataPlot.h"
#include "Application.h"
#include "DataRecorder.h"

RawDataPlot::RawDataPlot(QScrollArea *parent, Application &app) : QWidget(parent),
	dataRecorder(app.dataRecorder)
{
	scrollArea = parent;

	connect(&app.dataRecorder, SIGNAL(sampleAdded()), this, SLOT(onSampleAdded()));
	connect(&app.dataRecorder, SIGNAL(samplesCleared()), this, SLOT(onSamplesCleared()));

//	setMinimumWidth(0);
//	setAutoFillBackground(true);
}

void RawDataPlot::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
//	painter.setRenderHint(QPainter::Antialiasing, true);

	QPen pen(Qt::black, 1, Qt::SolidLine);
	painter.setPen(pen);

	int height = parentWidget()->height();

	painter.drawLine(0, height/2, width(), height/2);

	QRect rect = event->rect();

	int x0 = rect.left() - 1;
	int x1 = rect.right() + 1;

	if( x0 >= dataRecorder.size() )
		return;

	if( x0 < 0 )
		x0 = 0;
	if( x1 > dataRecorder.size() )
		x1 = dataRecorder.size();

	for(int i = x0 + 1; i < x1; ++i)
	{
		painter.drawLine(i-1, (height * (4095 - dataRecorder.at(i-1).xAccel)) / 4095,
				 i, (height * (4095 - dataRecorder.at(i).xAccel)) / 4095);
	}

//	painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());
}

void RawDataPlot::onSampleAdded()
{
	setMinimumWidth(dataRecorder.size());
	scrollArea->ensureVisible(dataRecorder.size(),0,1,1);

	update(dataRecorder.size()-2, 0, 5, parentWidget()->height());
}

void RawDataPlot::onSamplesCleared()
{
	setMinimumWidth(0);
	update(0, 0, parentWidget()->width(), parentWidget()->height());
}
