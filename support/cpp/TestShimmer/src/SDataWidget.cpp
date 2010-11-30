#include "SDataWidget.h"
#include "Application.h"
#include "ui_SDataWidget.h"
#include <QTreeWidget>
#include <QMessageBox>

SDataWidget::SDataWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDataWidget),
        application(app)
{

    ui->setupUi(this);
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(on_itemSelectionChanged()));

    // TODO Move it to a separate function
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

    int id=records[0].moteID;
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->sdataLeft->invisibleRootItem());
    item->setText(0,QString::number(records[0].moteID));
    for(int i=0; i<records.size(); i++){        
        if(!(records[i].moteID == id)){
            item = new QTreeWidgetItem(ui->sdataLeft->invisibleRootItem());
            item->setText(0,QString::number(records[i].moteID));
        }
              // TODO Remove duplication
            QTreeWidgetItem *it = new QTreeWidgetItem(item);
            it->setText(1,QString::number(records[i].num));
            it->setText(2,QString::number(records[i].length));
            it->setText(3,records[i].tor);
            it->setText(4,records[i].tod);

        id = records[i].moteID;
    }
}

SDataWidget::~SDataWidget()
{
    delete ui;
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

        getLinkingRecords(ui->sdataLeft->currentItem()->parent()->text(0).toInt(), ui->sdataLeft->currentItem()->text(1).toInt());
    }
}

// FIXME It actually ADDS the records
void SDataWidget::getLinkingRecords(int moteId, int num)
{
    for(int i=0; i<records.size(); i++){
        if(records[i].moteID == moteId-1){
            if(records[i].num == num){
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->sdataRight->invisibleRootItem());
                item->setText(0,QString::number(records[i].moteID));

              // TODO Remove duplication
                QTreeWidgetItem *it = new QTreeWidgetItem(item);
                it->setText(1,QString::number(records[i].num));
                it->setText(2,QString::number(records[i].length));
                it->setText(3,records[i].tor);
                it->setText(4,records[i].tod);
            }
        }

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
