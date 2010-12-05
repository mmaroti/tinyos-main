#ifndef DUMMY_HPP
#define DUMMY_HPP

#include <QThread>
#include <QDebug>

class SDataWidget;

class Dummy : public QThread {

    Q_OBJECT

public:

    void registerConnection(SDataWidget* widget);

    void startDownloading();

signals:

    void downloadFinished();

private:

    virtual void run();
};

#endif // DUMMY_HPP
