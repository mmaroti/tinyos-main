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
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(ui->sdataLeft, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
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

void SDataWidget::onItemSelectionChanged()
{
    ui->sdataRight->clear();

    if(ui->sdataLeft->currentItem()->parent()){

        const QString mote_id(ui->sdataLeft->currentItem()->parent()->text(0));
        const QString rec_id(ui->sdataLeft->currentItem()->text(1));

        qDebug() << "mote id: " << mote_id << ", record id: " << rec_id;

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

    if(ui->sdataRight->selectedItems().size() > 0){
        if(ui->sdataRight->currentItem()->parent()){

            QString msg;

            msg.append(ui->sdataLeft->currentItem()->parent()->text(0)+": ");
            msg.append(ui->sdataLeft->currentItem()->text(1));

            msg.append('\n'+ui->sdataRight->currentItem()->parent()->text(0)+": ");
            msg.append(ui->sdataRight->currentItem()->text(1));

            QMessageBox msgBox;

            msgBox.setText(msg);
            msgBox.exec();
        }
    }
}

void SDataWidget::on_clearButton_clicked()
{
    disconnect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    ui->sdataLeft->clear();
    ui->sdataRight->clear();
    records.clear();
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
}

void SDataWidget::processBinaryFile(const QString& dialogCaption, const QString& startFromHere, const QString& blockTitle) {

    QString file = selectBinaryFile(dialogCaption, startFromHere);

    if (file.size()==0) {

        return;
    }

    QString text = blockTitle + ", please wait...";

    showBlockingBox(blockTitle, text);

    manager.startProcessingFile(file, this);
}

void SDataWidget::downloadFromDevice() {

    QString device = selectWin32Device();

    if (device.size()==0) {

        return;
    }

    showBlockingBox("Downloading", "Downloading, please wait...");

    manager.startDownloading(device, this);
}

void SDataWidget::showBlockingBox(const QString& title, const QString& text) {

    blockingBox->setModal(true);
    blockingBox->setStandardButtons(QMessageBox::NoButton);
    blockingBox->setWindowTitle(title);
    blockingBox->setText(text);
    blockingBox->show();
}

const QString SDataWidget::selectWin32Device() {

    QFileDialog driveDialog(this, "Select drive");

    driveDialog.setFileMode(QFileDialog::DirectoryOnly);
    driveDialog.setViewMode(QFileDialog::List);
    driveDialog.setFilter(QDir::Drives);
    driveDialog.setDirectory("My Computer");
    driveDialog.exec();

    QStringList dirList = driveDialog.selectedFiles();
    QString device = dirList.at(0);

    qDebug() << device;

    return device;
}

const QString SDataWidget::selectBinaryFile(const QString& caption,
                                            const QString& startFromHere)
{
    return QFileDialog::getOpenFileName(this, caption, startFromHere);
}

void SDataWidget::on_downloadButton_clicked()
{

#ifdef _WIN32            
    downloadFromDevice();
#else
    processBinaryFile("Select the device", QDir::rootPath(), "Downloading");
#endif

}

void SDataWidget::on_fileButton_clicked()
{
    processBinaryFile("Select the binary file", QDir::homePath(), "Processing file");
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

void SDataWidget::onItemDoubleClicked(QTreeWidgetItem* item,int t)
{
    if(t == 5){
        item->setFlags( item->flags() | Qt::ItemIsEditable);
        ui->sdataLeft->editItem(item, t);
        item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }

}
