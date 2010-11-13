#include "SDownloadWidget.h"
#include "Application.h"
#include "ui_SDownloadWidget.h"
#include <QFileDialog>

SDownloadWidget::SDownloadWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDownloadWidget),
        application(app)
{
    ui->setupUi(this);
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
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "/home",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
}
