#ifndef DUMMY_HPP
#define DUMMY_HPP

#include <QThread>
#include <QDebug>

class SDataWidget;

class Dummy : public QThread {

    Q_OBJECT

public:

    Dummy(SDataWidget& sdata);
    ~Dummy();

    void registerConnection();

    void startDownloading();

signals:

    void downloadFinished();

private:
    SDataWidget& sdataWidget;
    virtual void run();
};

#endif // DUMMY_HPP
