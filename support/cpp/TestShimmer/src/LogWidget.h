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

    void createItem(QString text);

private slots:
    void on_entryLine_editingFinished();
};

#endif // LOGWIDGET_H
