#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QSignalMapper>

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
    QSignalMapper* signalMapper;

    void createItem(QString text);

private slots:
    void on_entryLine_returnPressed();
    void on_felvKezdButton_clicked();
    void on_felvVegButton_clicked();
    void on_mozgKezdButton_clicked();
    void on_mozgVegButton_clicked();
    void setTableEditable(bool);
    void onDelRow(int);
};

#endif // LOGWIDGET_H
