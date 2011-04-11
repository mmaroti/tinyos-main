#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;
class QPushButton;
class QLabel;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();

    void setTime(QString);
    void setSamples(QString);

Q_SIGNALS:
    void signalIntervalChanged(double);

public Q_SLOTS:
    void onConnectButtonPressed();

private:
    QPushButton *d_connectButton;
    WheelBox *d_intervalWheel;

    Plot *d_plot;

    QLabel* time;
    QLabel* samples;
};

#endif
