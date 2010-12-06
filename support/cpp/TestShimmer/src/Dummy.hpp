#ifndef DUMMY_HPP
#define DUMMY_HPP

#include <QThread>
#include <QVarLengthArray>
#include <QDebug>
#include "SDataWidget.h"

class Dummy : public QThread {

    Q_OBJECT

public:

    void registerConnection(SDataWidget* widget);

    void startDownloading();

signals:

    void downloadFinished(const QVarLengthArray<SData>& data);

private:

    virtual void run();

    QVarLengthArray<SData> data;
};

#endif // DUMMY_HPP
