#include "Dummy.hpp"

void Dummy::registerConnection(SDataWidget* widget) {

    qDebug() << "Connecting download slots";

    QObject::connect(this, SIGNAL(downloadFinished(const QVarLengthArray<SData>& )),
                     widget, SLOT(onDownloadFinished(const QVarLengthArray<SData>& )), Qt::DirectConnection);
}

void Dummy::startDownloading() {

    qDebug() << "Starting download";

    start();
}

void Dummy::run() {

    qDebug() << "It will take 3 seconds";

    msleep(3000);

    fillSData(data);

    emit downloadFinished(data);

    qDebug() << "Resources deleted";
}
