#ifndef RAWDATAPLOT_H
#define RAWDATAPLOT_H

#include <QWidget>
#include <QPen>

class QScrollArea;
class Application;
class DataRecorder;
class QPen;
class QTimerEvent;

class RawDataPlot : public QWidget
{
	Q_OBJECT
public:
	RawDataPlot(QScrollArea *parent, Application &app);

	enum
	{
		XACCEL = 0x0001,
		YACCEL = 0x0002,
		ZACCEL = 0x0004,
		XGYRO = 0x0008,
		YGYRO = 0x0010,
		ZGYRO = 0x0020,
		GRID = 0x0040,
		TIME = 0x0080,
	};

	void setGraphs(int graphs) { this->graphs = graphs; }
	int getGraphs() const { return graphs; }

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void timerEvent(QTimerEvent *event);

	int graphs;

public slots:
	void onSampleAdded();
	void onSamplesCleared();

private:
	DataRecorder &dataRecorder;
	QScrollArea *scrollArea;
	int parentHeight;
	int plotWidth;

	QPoint getPoint(int x, int y);
};

#endif // RAWDATAPLOT_H
