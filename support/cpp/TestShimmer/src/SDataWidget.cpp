#include "SDataWidget.h"
#include "Application.h"
#include "ui_SDataWidget.h"
#include <QTreeWidget>
#include <QMessageBox>
#include "SDownloadWidget.h"

SDataWidget::SDataWidget(QWidget *parent, Application &app, SDownloadWidget &sdata) :
        QWidget(parent),
        ui(new Ui::SDataWidget),
        application(app),
        sdownload(sdata)
{

    ui->setupUi(this);
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(on_itemSelectionChanged()));

    // TODO Move it to a separate function just like in SDownloadWidget
    initLeft();
}

SDataWidget::~SDataWidget()
{
    delete ui;
}

void SDataWidget::initLeft()
{
    int id=sdownload.getSDataAt(0).moteID;
    QTreeWidgetItem *item = createParentItem(0, ui->sdataLeft);
    for(int i=0; i<sdownload.size(); i++){
        if(!(sdownload.getSDataAt(i).moteID == id)){
            item = createParentItem(i, ui->sdataLeft);
        }
        createChildItem(i, item);

        id = sdownload.getSDataAt(i).moteID;
    }
}

void SDataWidget::initRight(QVarLengthArray<int> list)
{
    for(int i=0; i<list.size(); i++){
        QTreeWidgetItem* item = createParentItem(i, ui->sdataRight);
        createChildItem(i, item);
    }
}

void SDataWidget::on_itemSelectionChanged()
{
    ui->sdataRight->clear();
    QMessageBox msgBox;
    QString msg;
    if(!(ui->sdataLeft->currentItem()->parent()<ui->sdataLeft->invisibleRootItem())){
        msg.append(ui->sdataLeft->currentItem()->parent()->text(0));
        msg.append("\n"+ui->sdataLeft->currentItem()->text(2));
        msgBox.setText(msg);
        msgBox.exec();

        initRight(getLinkingRecords(ui->sdataLeft->currentItem()->parent()->text(0).toInt(), ui->sdataLeft->currentItem()->text(1).toInt()));
    }
}

// FIXME It actually ADDS the records
QVarLengthArray<int> SDataWidget::getLinkingRecords(int moteId, int num)
{
    QVarLengthArray<int> list;
    for(int i=0; i<sdownload.size(); i++){
        if(sdownload.getSDataAt(i).moteID == moteId-1){
            if(sdownload.getSDataAt(i).num == num){
                list.append(i);                
            }
        }
    }
    return list;
}

QTreeWidgetItem* SDataWidget::createParentItem(int i, QTreeWidget *root)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(root->invisibleRootItem());
    item->setText(0,QString::number(sdownload.getSDataAt(i).moteID));

    return item;
}

void SDataWidget::createChildItem(int i, QTreeWidgetItem* parent)
{
    QTreeWidgetItem *it = new QTreeWidgetItem(parent);
    it->setText(1,QString::number(sdownload.getSDataAt(i).num));
    it->setText(2,QString::number(sdownload.getSDataAt(i).length));
    it->setText(3,sdownload.getSDataAt(i).tor);
    it->setText(4,sdownload.getSDataAt(i).tod);
}

void SDataWidget::on_toPlotButton_clicked()
{
    QMessageBox msgBox;
    QString msg;
    if(ui->sdataRight->selectedItems().size() > 0){
        if(!(ui->sdataRight->currentItem()->parent()<ui->sdataRight->invisibleRootItem())){
            msg.append(ui->sdataRight->currentItem()->parent()->text(0));
            msg.append("\n");
            msg.append(ui->sdataRight->currentItem()->text(2));
            msgBox.setText(msg);
            msgBox.exec();
        }
    }
}


