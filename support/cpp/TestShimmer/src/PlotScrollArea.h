#ifndef PLOTSCROLLAREA_H
#define PLOTSCROLLAREA_H

#include <QScrollArea>
#include <QCursor>

class QWidget;
class QResizeEvent;

class PlotScrollArea : public QAbstractScrollArea
{
Q_OBJECT
public:
	explicit PlotScrollArea(QWidget *parent = 0);

	QWidget *widget() const { return plotWidget; }
	void setWidget(QWidget *widget);
	QWidget *takeWidget();

	QRect widgetRect() const { return plotRect; }
	void setWidgetRect(QRect rect);
	void ensureVisible(int x, int y, int xmargin, int ymargin);

protected:
	void updateWidgetPosition();
	void updateScrollBars();
	void resizeEvent(QResizeEvent *e);
	void scrollContentsBy(int dx, int dy);

	bool event(QEvent *e);
	bool eventFilter(QObject *o, QEvent *e);

protected:
	QWidget *plotWidget;
	QRect plotRect;
        QCursor cursor;
};

#endif // PLOTSCROLLAREA_H
