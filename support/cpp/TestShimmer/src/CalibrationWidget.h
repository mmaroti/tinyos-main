#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>

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
};

#endif // CALIBRATIONWIDGET_H
