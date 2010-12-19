#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

class Application;

namespace Ui{
    class LogWidget;
}

class LogWidget : public QWidget {
    Q_OBJECT
public:
    LogWidget(QWidget *parent, Application &app);
    ~LogWidget();

private:
    Ui::LogWidget *ui;
    Application &application;

};

#endif // LOGWIDGET_H
