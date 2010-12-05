#include "Dummy.hpp"
#include "SDataWidget.h"

void Dummy::registerConnection(SDataWidget* widget) {

    qDebug() << "Connecting download slots";

    QObject::connect(this, SIGNAL(downloadFinished()), widget, SLOT(onDownloadFinished()), Qt::DirectConnection);
}

void Dummy::startDownloading() {

    qDebug() << "Starting download";

    start();
}

void Dummy::run() {

    qDebug() << "It will take 3 seconds";

    msleep(3000);

    emit downloadFinished();

    qDebug() << "Exiting from thread";
}
