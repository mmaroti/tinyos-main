#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H


#include <QWidget>

class Application;

namespace Ui {
    class ConsoleWidget;
}

class ConsoleWidget : public QWidget {
    Q_OBJECT
public:
    ConsoleWidget(QWidget *parent, Application &app);
    ~ConsoleWidget();
    void loadConsoleData();

public slots:
    void on_recieveConsolSignal(QString);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConsoleWidget *ui;
    Application &application;
};


#endif // CONSOLEWIDGET_H
