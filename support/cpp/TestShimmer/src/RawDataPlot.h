#ifndef RAWDATAPLOT_H
#define RAWDATAPLOT_H

#include <QWidget>

class QScrollArea;
class Application;
class DataRecorder;

class RawDataPlot : public QWidget
{
	Q_OBJECT
public:
	RawDataPlot(QScrollArea *parent, Application &app);

protected:
	virtual void paintEvent(QPaintEvent *event);

public slots:
	void onSampleAdded();
	void onSamplesCleared();

private:
	DataRecorder &dataRecorder;
	QScrollArea *scrollArea;
};

#endif // RAWDATAPLOT_H
