#include "SDataWidget.h"
#include "Application.h"
#include "ui_SDataWidget.h"
#include <QTreeWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <QTime>
#include <QDialog>
#include <QDebug>

SData::SData()
{
    moteID = 0;
    num = 0;
    length = 0;
    tor = "";
    tod = "";
}

void fillSData(QVarLengthArray<SData>& records)
{
    for(int i=1; i<=rand() % 10 + 1; i++){
        int numOfRecs = rand() % 3 + 1;
        for(int j=1; j<=numOfRecs; j++){
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

SDataWidget::SDataWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDataWidget),
        application(app)
{

    ui->setupUi(this);
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(on_itemSelectionChanged()));
    blockingBox = new QMessageBox(QMessageBox::Information,
                                  "Downloading",
                                  "Please wait, downloading...",
                                  QMessageBox::NoButton, this, 0);

    connect(this, SIGNAL(updateGUI()), this, SLOT(onUpdateGUI()), Qt::QueuedConnection);

    fillSData(records);
    initLeft();
}

SDataWidget::~SDataWidget()
{
    delete blockingBox;
    delete ui;
}

void SDataWidget::initLeft()
{
    QTreeWidgetItem *item = createParentItem(0, ui->sdataLeft);
    for(int i=0; i<getRecordsSize(); i++){
        if(ui->sdataLeft->findItems(QString::number(getSDataAt(i).moteID),0,0).size() == 0){
            item = createParentItem(i, ui->sdataLeft);
        } else {
            item = ui->sdataLeft->findItems(QString::number(getSDataAt(i).moteID),0,0)[0];
        }

        createChildItem(i, item);
    }
}

void SDataWidget::initRight(QVarLengthArray<int> list)
{   
    if(list.size() != 0){
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
        if(ui->sdataRight->topLevelItem(0)->childCount()==1){
            ui->sdataRight->setCurrentItem(ui->sdataRight->topLevelItem(0)->child(0));
        }
    }
}

void SDataWidget::on_itemSelectionChanged()
{
    ui->sdataRight->clear();
    QMessageBox msgBox;
    QString msg;
    if(ui->sdataLeft->currentItem()->parent()){
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
    for(int i=0; i< getRecordsSize(); i++){
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
    item->setText(4, QDate::currentDate().toString() + "  -  " + QTime::currentTime().toString());

    return item;
}

void SDataWidget::createChildItem(int i, QTreeWidgetItem* parent)
{
    QTreeWidgetItem *it = new QTreeWidgetItem(parent);
    int num = parent->childCount();
    it->setText(1,QString::number(num));
    it->setText(2,QString::number(getSDataAt(i).length));
    it->setText(3,getSDataAt(i).tor);
    it->setText(4,getSDataAt(i).tod);
}

void SDataWidget::on_toPlotButton_clicked()
{
    QMessageBox msgBox;
    QString msg;
    if(ui->sdataRight->selectedItems().size() > 0){
        if(ui->sdataRight->currentItem()->parent()){
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
    ui->sdataLeft->clear();
    ui->sdataRight->clear();
    records.clear();
}

void SDataWidget::on_downloadButton_clicked()
{

#ifdef _WIN32
    QFileDialog driveDialog(this);
    driveDialog.setFileMode(QFileDialog::DirectoryOnly);
    driveDialog.setViewMode(QFileDialog::List);
    driveDialog.setFilter(QDir::Drives);
    driveDialog.setDirectory("My Computer");
    driveDialog.exec();
    QStringList dir = driveDialog.selectedFiles();
    qDebug() << dir[0];
#else
    QString dir = QFileDialog::getOpenFileName(this, "Select the device", "/dev");

#endif

    blockingBox->setModal(true);
    blockingBox->setStandardButtons(QMessageBox::NoButton);
    blockingBox->show();

    manager.startDownloading(dir, this);
}

void SDataWidget::onDownloadFinished(bool error, const QString& error_msg, const QVarLengthArray<SData>& data)
{
    downloadFailed = error;

    if (error) {

        errorMsg = QString(error_msg);
    }
    else {

        records = QVarLengthArray<SData>(data);
    }

    qDebug() << "Results of download copied";

    emit updateGUI();
}

void SDataWidget::onUpdateGUI() {

    if (downloadFailed) {
        QMessageBox mbox(QMessageBox::Warning, "Download failed", errorMsg);
        mbox.setText(errorMsg);
        blockingBox->hide();
        mbox.exec();
    }
    else {
        // FIXME Still buggy, it shows a msg box but it should not
        ui->sdataLeft->clear();
        ui->sdataRight->clear();
        initLeft();
        ui->sdataLeft->update();
        blockingBox->hide();
    }

    qDebug() << "GUI updated";
}

void SDataWidget::printRecords()
{
    for(int i=0; i<records.size(); i++){
        qDebug() << records[i].moteID << ", " << records[i].num << ", " << records[i].length << "\n";
    }
}
