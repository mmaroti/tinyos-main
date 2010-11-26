#ifndef SDOWNLOADWIDGET_H
#define SDOWNLOADWIDGET_H

#include <QWidget>
#include "XmlStreamReader.h"

class Application;

namespace Ui {
    class SDownloadWidget;
}

class SDownloadWidget : public QWidget {
    Q_OBJECT
public:
    SDownloadWidget(QWidget *parent, Application &app);
    ~SDownloadWidget();

private:
    Ui::SDownloadWidget *ui;
    Application &application;

    XmlStreamReader *reader;

    void setFlatFileModel(const QString &filename);
    QWidget setupListTab();

private slots:
    void on_downloadButton_clicked();
    void on_clearButton_clicked();
};

#endif // SDOWNLOADWIDGET_H
