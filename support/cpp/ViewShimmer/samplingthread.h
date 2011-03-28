#ifndef SAMPLINGTHREAD_H
#define SAMPLINGTHREAD_H

#include <qwt_sampling_thread.h>

class Application;

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread(QObject *parent, Application &app);

    double frequency() const;
    double amplitude() const;

public Q_SLOTS:
    void setAmplitude(double);
    void setFrequency(double);

protected:
    virtual void sample(double elapsed);

private:
    virtual double value(double timeStamp) const;

    double d_frequency;
    double d_amplitude;

    Application &application;
};

#endif
