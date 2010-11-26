#include "SDownloadWidget.h"
#include "Application.h"
#include "ui_SDownloadWidget.h"
#include <QFileDialog>
#include <QDebug>
#include "FlatFileModel.h"
#include <QListView>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "XmlStreamReader.h"

SDownloadWidget::SDownloadWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDownloadWidget),
        application(app)
{
    ui->setupUi(this);
    ui->sdDataView->setAlternatingRowColors(true);
    ui->sdDataView->setSortingEnabled(true);

    XmlStreamReader reader(ui->sdDataView);


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
        QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/", "Any File (*.*)");
        if ( !file.isEmpty() ) {
            reader->readFile(file);
            //something something DEESTROOY
        }
    #endif
}

void SDownloadWidget::setFlatFileModel(const QString &filename)
{
    //ui->sdDataView->setModel(new FlatFileModel(filename));
}

