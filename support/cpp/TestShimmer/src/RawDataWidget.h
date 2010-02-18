#ifndef RAWDATAWIDGET_H
#define RAWDATAWIDGET_H

#include <QWidget>

class Application;
class DataRecorder;

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

private slots:
	void on_clearButton_clicked();
	void on_recordButton_clicked();
};

#endif // RAWDATAWIDGET_H
