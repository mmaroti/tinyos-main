#include <QFileDialog>
#include <QDebug>
#include <QTreeWidget>
#include <QTableWidget>
#include "SDownloadWidget.h"
#include "Application.h"
#include "ui_SDownloadWidget.h"
#include <QMessageBox>

SDownloadWidget::SDownloadWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDownloadWidget),
        application(app)
{
    ui->setupUi(this);
    ui->sdDataView->setAlternatingRowColors(true);
    ui->sdDataView->setSortingEnabled(true);

    int row = 10;
    int column = 2;
    ui->sdDataView->setRowCount(row);
    ui->sdDataView->setColumnCount(column);
    QTableWidgetItem *newItem = new QTableWidgetItem("1");
            ui->sdDataView->setItem(0, 0, newItem);
    QTableWidgetItem *item2 = new QTableWidgetItem("2009.11.21 16:43 23:00");
            ui->sdDataView->setItem(0,1,item2);

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
    QMessageBox msgBox;
    #ifdef _WIN32
    //QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/", "Any File (*.*)");
    //if ( !file.isEmpty() ) {


   // }
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "c:/",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()){
            msgBox.setText(dir);
            //something something DAARK SIDE
        }
    #else
        QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open");
        if ( !file.isEmpty() ) {
            msgBox.setText(file);
            //something something DEESTROOY
        }
    #endif
        msgBox.exec();
}

void SDownloadWidget::setFlatFileModel(const QString &filename)
{
    //ui->sdDataView->setModel(new FlatFileModel(filename));
}

