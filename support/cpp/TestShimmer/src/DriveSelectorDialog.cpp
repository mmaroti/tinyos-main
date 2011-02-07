#include "DriveSelectorDialog.h"
#include "ui_DriveSelectorDialog.h"
#include <QFileInfoList>
#include <QDir>

DriveSelectorDialog::DriveSelectorDialog(QWidget *parent) : QDialog(parent),
    ui(new Ui::DriveSelectorDialog)
{
    ui->setupUi(this);

    drive = "";

    QFileInfoList driveData = QDir::drives();
    for(int i=0; i<driveData.size(); i++){
        new QListWidgetItem(driveData.at(i).absoluteFilePath(), ui->driveList);
    }

    connect( ui->cancelButton ,SIGNAL(clicked()), this ,SLOT(close()));
    connect( ui->selectButton, SIGNAL(clicked()), this, SLOT(accept()));

}

void DriveSelectorDialog::on_driveList_itemSelectionChanged()
{
    drive = ui->driveList->selectedItems().at(0)->text();
}

