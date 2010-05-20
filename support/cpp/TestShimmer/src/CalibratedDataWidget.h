#ifndef CALIBRATEDDATAWIDGET_H
#define CALIBRATEDDATAWIDGET_H

#include <QWidget>

class Application;
class DataRecorder;
class CalibratedDataPlot;

namespace Ui {
        class CalibratedDataWidget;
}

class CalibratedDataWidget : public QWidget {
    Q_OBJECT
public:
    CalibratedDataWidget(QWidget *parent, Application &app);
    ~CalibratedDataWidget();

protected:
        void changeEvent(QEvent *e);

private:
        Ui::CalibratedDataWidget *ui;
        Application &application;
        CalibratedDataPlot *plot;

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

#endif // CALIBRATEDDATAWIDGET_H
