#ifndef RAWDATAPLOT_H
#define RAWDATAPLOT_H

#include <QWidget>
#include <QPen>

class PlotScrollArea;
class Application;
class DataRecorder;
class QPen;
class QTimerEvent;

class RawDataPlot : public QWidget
{
	Q_OBJECT
public:
	RawDataPlot(PlotScrollArea *parent, Application &app);

	enum
	{
		XACCEL = 0x0001,
		YACCEL = 0x0002,
		ZACCEL = 0x0004,
		XGYRO = 0x0008,
		YGYRO = 0x0010,
		ZGYRO = 0x0020,
		VOLTAGE = 0x0040,
		GRID = 0x0100,
		TIME = 0x0200,
                TEMP = 0x0400,
	};

	void setGraphs(int graphs, bool on);
	int getGraphs() const { return graphs; }

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent * event);

	int graphs;

public slots:
	void onSampleAdded();
	void onSamplesCleared();

private:
	Application &application;
	PlotScrollArea *scrollArea;
	int parentHeight;
	int plotWidth;

	QPoint getPoint(int x, int y);
	QPoint getSample(int x, int y);
};

#endif // RAWDATAPLOT_H
