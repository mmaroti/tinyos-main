#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "CalibrationModule.h"
#include "PeriodicalCalibrationModule.h"

class Application;

namespace Ui {
    class CalibrationWidget;
}

class CalibrationWidget : public QWidget {
    Q_OBJECT
public:
    CalibrationWidget(QWidget *parent, Application &app);
    ~CalibrationWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CalibrationWidget *ui;
    Application &application;
    CalibrationModule *calibrationModule;
    PeriodicalCalibrationModule *periodicalCalibrationModule;

private slots:
    void on_startButton_clicked();

signals:
    void calibrationDone();
};

#endif // CALIBRATIONWIDGET_H
