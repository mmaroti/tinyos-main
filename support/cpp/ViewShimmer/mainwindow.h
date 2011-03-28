#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;
class QLabel;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget * = NULL);

    void start();

    double amplitude() const;
    double frequency() const;
    double signalInterval() const;

    void setTime(QString);
    void setSamples(QString);

Q_SIGNALS:
    void amplitudeChanged(double);
    void frequencyChanged(double);
    void signalIntervalChanged(double);

private:

    Knob *d_frequencyKnob;
    Knob *d_amplitudeKnob;
    WheelBox *d_timerWheel;
    WheelBox *d_intervalWheel;

    Plot *d_plot;

    QLabel* time;
    QLabel* samples;
};

#endif
