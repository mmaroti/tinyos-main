#include "Dummy.hpp"
#include "SDataWidget.h"

Dummy::Dummy(SDataWidget& sdata) : sdataWidget(sdata)
{

}

Dummy::~Dummy()
{}

void Dummy::registerConnection()
{

    qDebug() << "Connecting download slots";

    QObject::connect(this, SIGNAL(downloadFinished()), &sdataWidget, SLOT(onDownloadFinished()), Qt::DirectConnection);
}

void Dummy::startDownloading() {

    qDebug() << "Starting download";

    start();
}

void Dummy::run() {

    qDebug() << "It will take 3 seconds";

    sdataWidget.fillSData();
    msleep(3000);

    emit downloadFinished();

    qDebug() << "Resources deleted";
}
