#include <QFileDialog>
#include <QDebug>
#include <QTreeWidget>
#include "SDownloadWidget.h"
#include "Application.h"
#include "ui_SDownloadWidget.h"

SDownloadWidget::SDownloadWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDownloadWidget),
        application(app)
{
    ui->setupUi(this);
    ui->sdDataView->setAlternatingRowColors(true);
    ui->sdDataView->setSortingEnabled(true);

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->sdDataView->invisibleRootItem());
    item->setText(0,"1");

    QTreeWidgetItem *it = new QTreeWidgetItem(item);
    it->setText(1,"1");
    it->setText(2,"3675");
    it->setText(3,"2010-10-30 12:00 00:00");
    it->setText(4,"2010-11-01 11:34 00:00");

}

SDownloadWidget::~SDownloadWidget()
{
    delete ui;
}

void SDownloadWidget::on_clearButton_clicked()
{
}

void SDownloadWidget::on_downloadButton_clicked()
{
    #ifdef _WIN32
    QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/", "Any File (*.*)");
    if ( !file.isEmpty() ) {


    }
        /*QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "/home",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()){
            //something something DAARK SIDE
        }*/
    #else
    // TODO On Linux we do not have C:/
    // TODO Do not use filter, *.* assumes that we have extension
        QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/", "Any File (*.*)");
        if ( !file.isEmpty() ) {

            //something something DEESTROOY
        }
    #endif
}

void SDownloadWidget::setFlatFileModel(const QString &filename)
{
    //ui->sdDataView->setModel(new FlatFileModel(filename));
}

