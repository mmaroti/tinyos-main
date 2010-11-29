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

    QTreeWidgetItem *item = new QTreeWidgetItem(ui->sdataLeft->invisibleRootItem());
    item->setText(0,"1");

    QTreeWidgetItem *it = new QTreeWidgetItem(item);
    it->setText(1,"1");
    it->setText(2,"3675");
    it->setText(3,"2010-10-30 12:00 00:00");
    it->setText(4,"2010-11-01 11:34 00:00");
    QTreeWidgetItem *it2 = new QTreeWidgetItem(item);
    it2->setText(1,"2");
    it2->setText(2,"1423");
    it2->setText(3,"2010-10-30 13:14 22:00");
    it2->setText(4,"2010-11-01 11:34 00:00");

    QTreeWidgetItem *item2 = new QTreeWidgetItem(ui->sdataLeft->invisibleRootItem());
    item2->setText(0,"2");

    QTreeWidgetItem *it21 = new QTreeWidgetItem(item2);
    it21->setText(1,"1");
    it21->setText(2,"235");
    it21->setText(3,"2010-10-30 21:54 12:00");
    it21->setText(4,"2010-11-01 11:34 00:00");
}

SDataWidget::~SDataWidget()
{
    delete ui;
}

void SDataWidget::on_itemSelectionChanged()
{
    QMessageBox msgBox;
    QString msg;
    if(ui->sdataLeft->currentItem()->parent()<ui->sdataLeft->invisibleRootItem()){
        msg.append(ui->sdataLeft->currentItem()->text(0));
    }else{
        msg.append(ui->sdataLeft->currentItem()->parent()->text(0));
    }

    msg.append("\n valami int");
    msgBox.setText(msg);
    msgBox.exec();
}
