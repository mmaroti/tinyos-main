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

    // TODO Move it to a separate init() function, init() should call another funtion for the data
    fillSData();
    init();

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
        }
    #else
        QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open");
        if ( !file.isEmpty() ) {
            msgBox.setText(file);
        }
    #endif
        msgBox.exec();
}

void SDownloadWidget::setFlatFileModel(const QString &)
{
    //ui->sdDataView->setModel(new FlatFileModel(filename));
}

void SDownloadWidget::init()
{
    int row = size();
    int column = 2;
    ui->sdDataView->setRowCount(row);
    ui->sdDataView->setColumnCount(column);
    for(int i=0; i<row; i++){
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(getSDataAt(i).moteID));
                ui->sdDataView->setItem(i, 0, item1);
        QTableWidgetItem *item2 = new QTableWidgetItem(getSDataAt(i).tod);
                ui->sdDataView->setItem(i, 1, item2);
    }
}

SData::SData()
{
    moteID = 0;
    num = 0;
    length = 0;
    tor = "";
    tod = "";
}

void SDownloadWidget::fillSData()
{
    for(int i=1; i<10; i++){
        for(int j=1; j<4; j++){
            SData record;

            record.moteID = i;
            record.num = j;
            record.length = rand()*1000;
            record.tod = "2010-10-30 12:00 00:00";
            record.tor = "2010-11-01 11:34 00:00";

            records.append(record);
        }
    }
}

