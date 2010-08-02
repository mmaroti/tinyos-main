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

#include "PlotScrollArea.h"
#include <QScrollBar>
#include <QStyle>
#include <QEvent>

PlotScrollArea::PlotScrollArea(QWidget *parent) :
    QAbstractScrollArea(parent)
{

	plotWidget = NULL;
        cursor.setShape(Qt::CrossCursor);
	viewport()->setBackgroundRole(QPalette::NoRole);
        viewport()->setCursor(cursor);

	horizontalScrollBar()->setSingleStep(10);
	verticalScrollBar()->setSingleStep(10);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void PlotScrollArea::setWidgetRect(QRect rect)
{
	plotRect = rect;
	updateScrollBars();
}

void PlotScrollArea::updateWidgetPosition()
{
	plotWidget->move(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
}

void PlotScrollArea::updateScrollBars()
{
	if( plotWidget == NULL )
		return;

	QSize p = viewport()->size();
	QRect rect = plotRect;

	horizontalScrollBar()->setRange(rect.left(), rect.right()-p.width());
	horizontalScrollBar()->setPageStep(p.width());
	verticalScrollBar()->setRange(rect.top(), rect.bottom()-p.height());
	verticalScrollBar()->setPageStep(p.height());
	updateWidgetPosition();
}

/**
 * Same function as in QScrollArea
 */
void PlotScrollArea::setWidget(QWidget *widget)
{
	if( plotWidget != NULL )
	{
		delete plotWidget;
		plotWidget = NULL;
		horizontalScrollBar()->setValue(0);
		verticalScrollBar()->setValue(0);
	}

	if( widget != NULL )
	{
		plotWidget = widget;
		if( widget->parentWidget() != viewport() )
			widget->setParent(viewport());
		if( !widget->testAttribute(Qt::WA_Resized) )
			widget->resize(widget->sizeHint());
		widget->setAutoFillBackground(true);
		widget->installEventFilter(this);
		updateScrollBars();
		widget->show();
	}
}

/**
 * Same function as in QScrollArea
 */
QWidget *PlotScrollArea::takeWidget()
{
	QWidget *widget = plotWidget;
	plotWidget = NULL;
	if(widget != NULL )
		widget->setParent(NULL);
	return widget;
}

/**
 * Same function as in QScrollArea
 */
void PlotScrollArea::ensureVisible(int x, int y, int xmargin, int ymargin)
{
	if( x - xmargin < horizontalScrollBar()->value() )
		horizontalScrollBar()->setValue(qMax(horizontalScrollBar()->minimum(), x - xmargin));
	else if( x + xmargin > horizontalScrollBar()->value() + viewport()->width() )
		horizontalScrollBar()->setValue(qMin(horizontalScrollBar()->maximum(), x + xmargin - viewport()->width()));

	if( y - ymargin < verticalScrollBar()->value() )
		verticalScrollBar()->setValue(qMax(verticalScrollBar()->minimum(), y - ymargin));
	else if( y + ymargin > horizontalScrollBar()->value() + viewport()->height() )
		verticalScrollBar()->setValue(qMin(verticalScrollBar()->maximum(), y + ymargin - viewport()->height()));
}

void PlotScrollArea::resizeEvent(QResizeEvent *)
{
	updateScrollBars();
}

void PlotScrollArea::scrollContentsBy(int, int)
{
	if( plotWidget == NULL )
		return;

	updateWidgetPosition();
}

bool PlotScrollArea::event(QEvent *e)
{
	if( e->type() == QEvent::StyleChange || e->type() == QEvent::LayoutRequest )
	{
		updateScrollBars();
	}

	return QAbstractScrollArea::event(e);
}

bool PlotScrollArea::eventFilter(QObject *o, QEvent *e)
{
	if( o == widget() && e->type() == QEvent::Resize )
	{
		updateScrollBars();
	}

	return false;
}
