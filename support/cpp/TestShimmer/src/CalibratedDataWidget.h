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

        void on_zAccel_clicked();
        void on_yAccel_clicked();
        void on_xAccel_clicked();
        void on_avgAccel_clicked();
        void on_xyangle_clicked();
        void on_yzangle_clicked();
        void on_zxangle_clicked();

        void on_exportButton_clicked();
};

#endif // CALIBRATEDDATAWIDGET_H
