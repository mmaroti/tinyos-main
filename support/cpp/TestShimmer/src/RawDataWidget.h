#ifndef RAWDATAWIDGET_H
#define RAWDATAWIDGET_H

#include <QWidget>
#include "CalibratedDataPlot.h"

class Application;
class DataRecorder;
class RawDataPlot;

namespace Ui {
	class RawDataWidget;
}

class RawDataWidget : public QWidget {
	Q_OBJECT
public:
	RawDataWidget(QWidget *parent, Application &app);
	~RawDataWidget();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::RawDataWidget *ui;
	Application &application;
	RawDataPlot *plot;

private slots:
        void on_temp_clicked();
	void on_voltage_clicked();
	void on_zGyro_clicked();
	void on_yGyro_clicked();
	void on_xGyro_clicked();
	void on_zAccel_clicked();
	void on_yAccel_clicked();
	void on_xAccel_clicked();
	void on_clearButton_clicked();
	void on_recordButton_clicked();
        void on_loadButton_clicked();
        void on_saveButton_clicked();
};

#endif // RAWDATAWIDGET_H
