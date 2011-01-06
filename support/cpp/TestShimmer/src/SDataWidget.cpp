/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Péter Ruzicska
*
*/

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
#include "constants.h"
#include "RecordList.hpp"
#include "MoteHeader.hpp"
#include "RecordLine.hpp"


SDataWidget::SDataWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::SDataWidget),
        application(app)
{
    ui->setupUi(this);
    //this->setMouseTracking(true);
    //ui->sdataLeft->installEventFilter(this);
    connect(ui->sdataLeft, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(ui->sdataLeft, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
    blockingBox = new QMessageBox(QMessageBox::Information,
                                  "Downloading",
                                  "Please wait, downloading...",
                                  QMessageBox::NoButton, this, 0);

    connect(this, SIGNAL(updateGUI()), this, SLOT(onUpdateGUI()), Qt::QueuedConnection);

    //fillSData(records);
    //initLeft(false);
}

SDataWidget::~SDataWidget()
{
    delete blockingBox;
    delete ui;
}

void SDataWidget::initLeft(bool filter)
{
    recordList.read_all_existing();    
    int k = 0;
    for(int i=0; i<recordList.headers().size(); i++){
        QTreeWidgetItem *item = createParentItem(i, ui->sdataLeft);
        int records = recordList.headers().at(i).number_of_records() + k;
        while(k<records){
            if(filter){
                if(item->childCount()<NUMOFRECS) createChildItem(k, item);
            } else {
                createChildItem(k, item);
            }
            k++;
        }
    }
}

void SDataWidget::initRight(const QVarLengthArray<int>& list)
{   
//    if(list.size() != 0){
//        int id=getSDataAt(list[0]).moteID;
//        QTreeWidgetItem *item = createParentItem(list[0], ui->sdataRight);
//
//        for(int i=0; i<list.size(); i++){
//            if(!(getSDataAt(list[i]).moteID == id)){
//                item = createParentItem(list[i], ui->sdataLeft);
//            }
//            createChildItem(list[i], item);
//
//            id = getSDataAt(list[i]).moteID;
//        }
//        ui->sdataRight->expandAll();
//        if(ui->sdataRight->topLevelItem(0)->childCount()==1){
//            ui->sdataRight->setCurrentItem(ui->sdataRight->topLevelItem(0)->child(0));
//        }
//    }
}

void SDataWidget::onItemSelectionChanged()
{
    ui->sdataRight->clear();

    if(ui->sdataLeft->currentItem()->parent()){

        const QString mote_id(ui->sdataLeft->currentItem()->parent()->text(0));
        const QString rec_id(ui->sdataLeft->currentItem()->text(1));

        qDebug() << "mote id: " << mote_id << ", record id: " << rec_id;
        recordList.search_for_matching_records(mote_id.toInt(), rec_id.toInt());

        initRight(getLinkingRecords(ui->sdataLeft->currentItem()->parent()->text(0).toInt(), ui->sdataLeft->currentItem()->text(1).toInt()));
    }
}

QVarLengthArray<int> SDataWidget::getLinkingRecords(int moteId, int num)
{
    QVarLengthArray<int> list;

    return list;
}

QTreeWidgetItem* SDataWidget::createParentItem(int i, QTreeWidget *root)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(root->invisibleRootItem());

    item->setData(MOTE_ID,       Qt::DisplayRole, recordList.headers().at(i).mote_id());
    item->setData(DATE_DOWNLOAD, Qt::DisplayRole, recordList.headers().at(i).last_download());
    item->setData(COMMENT,       Qt::DisplayRole, recordList.headers().at(i).remaining_hours() + " hours remaining");

    return item;
}

void SDataWidget::createChildItem(int i, QTreeWidgetItem* parent)
{
    QTreeWidgetItem *it = new QTreeWidgetItem(parent);
    //int num = parent->childCount();
    it->setData(RECORD_ID,     Qt::DisplayRole, recordList.record_info().at(i).record_id());
    it->setData(LENGTH,        Qt::DisplayRole, recordList.record_info().at(i).length());
    it->setData(DATE_OF_REC,   Qt::DisplayRole, recordList.record_info().at(i).recorded());
    it->setData(DATE_DOWNLOAD, Qt::DisplayRole, recordList.record_info().at(i).downloaded());
}

void SDataWidget::on_toPlotButton_clicked()
{

    if(ui->sdataRight->selectedItems().size() > 0){
        if(ui->sdataRight->currentItem()->parent()){

            QString msg;

            msg.append(ui->sdataLeft->currentItem()->parent()->text(MOTE_ID)+": ");
            msg.append(ui->sdataLeft->currentItem()->text(RECORD_ID));

            msg.append('\n'+ui->sdataRight->currentItem()->parent()->text(MOTE_ID)+": ");
            msg.append(ui->sdataRight->currentItem()->text(RECORD_ID));

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

void SDataWidget::onDownloadFinished(bool error, const QString& error_msg)
{
    downloadFailed = error;

    if (error) {

        errorMsg = QString(error_msg);
    }
    else {
        //TODO
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
        ui->sdataLeft->clear();
        ui->sdataRight->clear();
        initLeft(false);
        ui->sdataLeft->update();
        blockingBox->hide();
    }

    qDebug() << "GUI updated";
}

void SDataWidget::onItemDoubleClicked(QTreeWidgetItem* item,int column)
{
    if(column == COMMENT){
        item->setFlags( item->flags() | Qt::ItemIsEditable);
        ui->sdataLeft->editItem(item, column);
        item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }

}

void SDataWidget::on_showLastTencBox_clicked()
{
    if(ui->showLastTencBox->isChecked()){
        ui->sdataLeft->clear();
        initLeft(true);
        ui->sdataLeft->expandAll();
    } else {
        ui->sdataLeft->clear();
        initLeft(false);
    }
}

void SDataWidget::onSdataLeftFocusIn()
{
    ui->sdataLeft->clear();
    initLeft(false);
}
