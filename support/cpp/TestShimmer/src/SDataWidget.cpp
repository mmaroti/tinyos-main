#include "SDataWidget.h"
#include "Application.h"
#include "ui_SDataWidget.h"
#include <QTreeWidget>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

SData::SData()
{
    moteID = 0;
    num = 0;
    length = 0;
    tor = "";
    tod = "";
}

SDataWidget::SDataWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDataWidget),
        application(app)
{

    ui->setupUi(this);
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(on_itemSelectionChanged()));

    fillSData();
    initLeft();
}

SDataWidget::~SDataWidget()
{
    delete ui;
}

void SDataWidget::fillSData()
{
    for(int i=1; i<10; i++){
        for(int j=1; j<4; j++){
            SData record;

            record.moteID = i;
            record.num = j;
            record.length = rand()*1000;
            record.tod = "2010-10-30 12:00";
            record.tor = "2010-11-01 11:34";

            records.append(record);
        }
    }
}

void SDataWidget::initLeft()
{
    int id=getSDataAt(0).moteID;
    QTreeWidgetItem *item = createParentItem(0, ui->sdataLeft);
    for(int i=0; i<size(); i++){
        if(!(getSDataAt(i).moteID == id)){
            item = createParentItem(i, ui->sdataLeft);
        }
        createChildItem(i, item);

        id = getSDataAt(i).moteID;
    }
}

void SDataWidget::initRight(QVarLengthArray<int> list)
{    
    int id=getSDataAt(list[0]).moteID;
    QTreeWidgetItem *item = createParentItem(list[0], ui->sdataRight);
    for(int i=0; i<list.size(); i++){
        if(!(getSDataAt(list[i]).moteID == id)){
            item = createParentItem(list[i], ui->sdataLeft);
        }
        createChildItem(list[i], item);

        id = getSDataAt(list[i]).moteID;
    }
    ui->sdataRight->expandAll();
}

void SDataWidget::on_itemSelectionChanged()
{
    ui->sdataRight->clear();
    QMessageBox msgBox;
    QString msg;
    if(!(ui->sdataLeft->currentItem()->parent()<ui->sdataLeft->invisibleRootItem())){
        msg.append(ui->sdataLeft->currentItem()->parent()->text(0));
        msg.append("\n"+ui->sdataLeft->currentItem()->text(1));
        msgBox.setText(msg);
        msgBox.exec();

        initRight(getLinkingRecords(ui->sdataLeft->currentItem()->parent()->text(0).toInt(), ui->sdataLeft->currentItem()->text(1).toInt()));
    }
}

QVarLengthArray<int> SDataWidget::getLinkingRecords(int moteId, int num)
{
    QVarLengthArray<int> list;
    for(int i=0; i<size(); i++){
        if(getSDataAt(i).moteID == moteId-1){
            if(getSDataAt(i).num >= num){
                list.append(i);                
            }
        }
    }
    return list;
}

QTreeWidgetItem* SDataWidget::createParentItem(int i, QTreeWidget *root)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(root->invisibleRootItem());
    item->setText(0,QString::number(getSDataAt(i).moteID));

    return item;
}

void SDataWidget::createChildItem(int i, QTreeWidgetItem* parent)
{
    QTreeWidgetItem *it = new QTreeWidgetItem(parent);
    it->setText(1,QString::number(getSDataAt(i).num));
    it->setText(2,QString::number(getSDataAt(i).length));
    it->setText(3,getSDataAt(i).tor);
    it->setText(4,getSDataAt(i).tod);
}

void SDataWidget::on_toPlotButton_clicked()
{
    QMessageBox msgBox;
    QString msg;
    if(ui->sdataRight->selectedItems().size() > 0){
        if(!(ui->sdataRight->currentItem()->parent()<ui->sdataRight->invisibleRootItem())){
            msg.append(ui->sdataRight->currentItem()->parent()->text(0));
            msg.append("\n");
            msg.append(ui->sdataRight->currentItem()->text(1));

            msg.append("\n"+ui->sdataLeft->currentItem()->parent()->text(0)+"\n");
            msg.append(ui->sdataLeft->currentItem()->text(1));

            msgBox.setText(msg);
            msgBox.exec();
        }
    }
}

void SDataWidget::on_clearButton_clicked()
{
    records.clear();
    ui->sdataLeft->clear();
    ui->sdataRight->clear();
}

void SDataWidget::on_downloadButton_clicked()
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
        QString file = QFileDialog::getOpenFileName(this, "Select the device");
        if ( !file.isEmpty() ) {
            msgBox.setText(file);
        }
    #endif
        msgBox.exec();
}


