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
*/

#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "Application.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QMessageBox>
#include <QtDebug>
#include <QAction>
#include <QMenu>

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app)
{
    ui->setupUi(this);

//    delSignalMapper = new QSignalMapper(this);
//    gotoSignalMapper = new QSignalMapper(this);

    init();

    ui->log->setRowCount(0);
    ui->log->horizontalHeader()->resizeSection(0, 40);
    ui->log->horizontalHeader()->resizeSection(1, 70);
    ui->log->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(3, 40);

    ui->log->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ui->log->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

//    connect(delSignalMapper, SIGNAL(mapped(int)),this, SLOT(onDelRow(int)));
//    connect(gotoSignalMapper, SIGNAL(mapped(int)), this, SLOT(onGoto(int)));
}

LogWidget::~LogWidget()
{

}

void LogWidget::init()
{
    ui->log->clearContents();
    ui->log->setRowCount(0);

    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    ui->entryLine->setFocus();
}


void LogWidget::on_entryLine_returnPressed()
{
    if(!ui->entryLine->text().isNull()){
        createItem(ui->entryLine->text(), Text, -1);
    }
}

void LogWidget::createItem(QString text, Button button, int at)
{
    QString txt = "";
    int row;
    if(at == -1){
        row = ui->log->rowCount();
        ui->log->insertRow(row);
    } else {
        row = at;
        ui->log->insertRow(row);
    }


    if(button == RecordStart){
        txt = QDate::currentDate().toString();
        txt.append(" - "+text);
    } else {
        txt = text;
    }

    if(button == MotionStart){
        QTableWidgetItem* gotoButton = new QTableWidgetItem(QIcon(":/icons/back-arrow.png"),"", 0);
        ui->log->setItem(row, 0, gotoButton);
        gotoButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        QTableWidgetItem* gotoButton = new QTableWidgetItem("", 0);
        ui->log->setItem(row, 0, gotoButton);
        gotoButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    QTableWidgetItem* item = new QTableWidgetItem(txt,1);
    ui->log->setItem(row,2,item);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if(button == Insert){
        QTableWidgetItem* time = new QTableWidgetItem(ui->log->item(row-1,1)->text(),1);        
        ui->log->setItem(row,1,time);
        time->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        QTableWidgetItem* time = new QTableWidgetItem(QTime::currentTime().toString(),1);
        ui->log->setItem(row,1,time);
        time->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }



    if(button != RecordStart && button != RecordEnd){
        QTableWidgetItem* del = new QTableWidgetItem(QIcon(":/icons/Delete.png"),"", 0);
        ui->log->setItem(row, 3, del);
        del->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        QTableWidgetItem* del = new QTableWidgetItem("", 0);
        ui->log->setItem(row, 3, del);
        del->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidget::on_recStartButton_clicked()
{
    init();

    QString msg = QString::fromUtf8("Rec start");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,RecordStart,-1);
    ui->recStartButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
    ui->loadButton->setEnabled(false);

    ui->entryLine->setFocus();
}

void LogWidget::on_recEndButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to finish recording?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){
        QString msg = QString::fromUtf8("Rec End");
        if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
        createItem(msg,RecordEnd,-1);
        ui->recStartButton->setEnabled(false);
        ui->recEndButton->setEnabled(false);
        ui->motionStartButton->setEnabled(false);
        ui->entryLine->setEnabled(false);
        ui->saveButton->setEnabled(true);

        //disconnect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    }

    ui->entryLine->setFocus();
}

void LogWidget::on_motionStartButton_clicked()
{
    QString msg = QString::fromUtf8("Motion start");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,MotionStart,-1);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);
    ui->recEndButton->setEnabled(false);

    ui->entryLine->setFocus();
}

void LogWidget::on_motionEndButton_clicked()
{
    QString msg = QString::fromUtf8("Motion end");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,MotionEnd,-1);
    ui->motionEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->recEndButton->setEnabled(true);

    ui->entryLine->setFocus();
    ui->log->update();
}

void LogWidget::on_loadButton_clicked()
{
    init();

    ui->recStartButton->setEnabled(false);
    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
    ui->saveButton->setEnabled(true);

    ui->entryLine->setFocus();
}

void LogWidget::on_saveButton_clicked()
{
    ui->recStartButton->setEnabled(true);
    ui->loadButton->setEnabled(true);
    ui->entryLine->setEnabled(true);

    //connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

    ui->entryLine->setFocus();
}

void LogWidget::onDelRow(int row)
{
    int startRow = row;
    int endRow = row;

    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to delete this row?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    if(ui->log->item(row,2)->text().contains("Motion start", Qt::CaseSensitive)){
        endRow = findMotionEnd(row);

        msgBox.setText("WARNING! Deleting complete motion block!");
        msgBox.setInformativeText("Are you sure?");
        msgBox.setIcon(QMessageBox::Warning);
    } else if(ui->log->item(row,2)->text().contains("Motion end", Qt::CaseSensitive)){
        startRow = findMotionStart(row);

        msgBox.setText("WARNING! Deleting complete motion block!");
        msgBox.setInformativeText("Are you sure?");
        msgBox.setIcon(QMessageBox::Warning);
    }

    if(row != 0 && row != ui->log->rowCount()-1){
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok){
            for(int j=startRow; j<=endRow; j++ ){
                ui->log->removeRow(startRow);
            }
        }
    }

    ui->entryLine->setFocus();
}

void LogWidget::onGoto(int row)
{
    QMessageBox msgBox;
    QString msg = "Start - End\n";
    if(findMotionEnd(row) != -1)
        msg.append(ui->log->item(row,1)->text() + " - " + ui->log->item(findMotionEnd(row),1)->text());

    msgBox.setText(msg);
    msgBox.exec();
}

int LogWidget::findMotionStart(int endRow)
{
    int startRow = -1;

    for(int i = endRow; i > 0; i--){
        if(ui->log->item(i,2)->text().contains("Motion start", Qt::CaseSensitive)){
            startRow = i;
            break;
        }
    }

    return startRow;
}

int LogWidget::findMotionEnd(int startRow)
{
    int endRow = -1;

    for(int i = startRow; i < ui->log->rowCount(); i++){
        if(ui->log->item(i,2)->text().contains("Motion end", Qt::CaseSensitive)){
            endRow = i;
            break;
        }
    }

    return endRow;
}

int LogWidget::motionDistance(int startRow, int endRow)
{
    return endRow-startRow;
}

void LogWidget::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = ui->log->mapToGlobal(pos);

    int row = ui->log->rowAt(pos.y());

    QMenu myMenu;
    myMenu.addAction("Insert Row After");

    if(row != -1 && !isRecordEnd(row)){
        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem)
        {
            createItem(ui->entryLine->text(), Insert, row+1);

            ui->entryLine->setFocus();
        }
        else
        {
            // nothing was chosen
        }
    }
}

void LogWidget::on_log_cellClicked(int row, int column)
{
    if(column == 0){
        if(row == findMotionStart(row)) onGoto(row);
    } else if(column == 3){
        onDelRow(row);
    }
}

void LogWidget::on_log_itemChanged(QTableWidgetItem* item)
{
    int row = item->row();
    int column = item->column();
    if(column == 1 && item->isSelected() && row < ui->log->rowCount()){
        QTime before = QTime::fromString(ui->log->item(row-1,column)->text(), "hh:mm:ss");
        QTime after = QTime::fromString(ui->log->item(row+1,column)->text(), "hh:mm:ss");
        QTime now = QTime::fromString(ui->log->item(row,column)->text(), "hh:mm:ss");

        if(before > now || after < now){
            QMessageBox msgBox;
            msgBox.setText("Time value invalid!\nPlease enter a time value between\n"+ui->log->item(row-1,column)->text()+" - "+ui->log->item(row+1,column)->text());
            msgBox.exec();

            item->setSelected(true);
        }
    }
}

bool LogWidget::isRecordEnd(int row)
{
    return ui->log->item(row,2)->text().contains("Rec End", Qt::CaseSensitive);
}
