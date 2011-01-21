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
#include <QBrush>

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app)
{
    ui->setupUi(this);

    delSignalMapper = new QSignalMapper(this);
    gotoSignalMapper = new QSignalMapper(this);

    init();

    ui->log->setRowCount(0);
    ui->log->horizontalHeader()->resizeSection(0, 40);
    ui->log->horizontalHeader()->resizeSection(1, 70);
    ui->log->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(3, 40);

    ui->log->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->entryLine->setFocus();

    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);

    connect(delSignalMapper, SIGNAL(mapped(int)),this, SLOT(onDelRow(int)));
    connect(gotoSignalMapper, SIGNAL(mapped(int)), this, SLOT(onGoto(int)));
}

LogWidget::~LogWidget()
{

}

void LogWidget::init()
{
    id = 0;
    motionStarted = false;
    motionStart = -1;
    logMap.clear();
    ui->log->clearContents();
    ui->log->setRowCount(0);

    ui->entryLine->setFocus();
}


void LogWidget::on_entryLine_returnPressed()
{
    if(!ui->entryLine->text().isNull()){
        createItem(ui->entryLine->text(), false);
    }
}

void LogWidget::createItem(QString text, bool createArrow)
{
    QString txt = "";
    int row = ui->log->rowCount();
    ui->log->insertRow(row);

    if(row == 0){
        txt = QDate::currentDate().toString();
        txt.append(" - "+text);
    } else {
        txt = text;
    }

    if(createArrow){
        motionStart = id;
        QPushButton* gotoButton = new QPushButton(QIcon(":/icons/back-arrow.png"),"",this);
        gotoButton->setMaximumSize(40,20);
        ui->log->setCellWidget(row,0,gotoButton);

        gotoSignalMapper->setMapping(gotoButton, id);

        connect(gotoButton, SIGNAL(clicked()), gotoSignalMapper, SLOT (map()));
    }

    QTableWidgetItem* time = new QTableWidgetItem(QTime::currentTime().toString(),1);
    ui->log->setItem(row,1,time);

    QTableWidgetItem* item = new QTableWidgetItem(txt,1);
    ui->log->setItem(row,2,item);

    QPushButton* del = new QPushButton(QIcon(":/icons/Delete.png"),"",this);
    del->setMaximumSize(20,20);
    ui->log->setCellWidget(row,3,del);
    delSignalMapper->setMapping(del, id);

    connect(del, SIGNAL(clicked()), delSignalMapper, SLOT (map()));

    ui->entryLine->clear();
    ui->entryLine->setFocus();

    logMap.insert(id, row);
    id++;
}

void LogWidget::on_recStartButton_clicked()
{
    init();

    QString msg = QString::fromUtf8("Rec start");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,false);
    ui->recStartButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);

    ui->entryLine->setFocus();
}

void LogWidget::on_recEndButton_clicked()
{
    QString msg = QString::fromUtf8("Rec End");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,false);
    ui->recStartButton->setEnabled(false);
    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);

    ui->entryLine->setFocus();
}

void LogWidget::on_motionStartButton_clicked()
{
    motionStarted = false;
    QString msg = QString::fromUtf8("Motion start");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,true);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);

    ui->entryLine->setFocus();
}

void LogWidget::on_motionEndButton_clicked()
{
    motionStarted = false;
    QString msg = QString::fromUtf8("Motion end");
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg,false);
    ui->motionEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->recEndButton->setEnabled(true);

    ui->log->setSpan(logMap.value(motionStart),0, motionDistance(motionStart, findMotionEnd(motionStart)),1 );

    QBrush blue(Qt::blue);
    for(int i = motionStart; i <= findMotionEnd(motionStart); i++){
        ui->log->item(logMap.value(i),2)->setBackground(blue);
    }
    ui->log->item(logMap.value(motionStart),2)->setBackground(blue);

    ui->entryLine->setFocus();
    ui->log->update();
}

void LogWidget::on_loadButton_clicked()
{
    ui->recStartButton->setEnabled(false);
    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
}

void LogWidget::on_saveButton_clicked()
{
    ui->recStartButton->setEnabled(true);
}

void LogWidget::onDelRow(int id)
{
    int startId = id;
    int endId = id;

    QMessageBox msgBox;
    msgBox.setInformativeText("Are you sure you want to delete this row?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    if(ui->log->item(logMap.value(id),2)->text().contains("Motion start", Qt::CaseSensitive)){
        endId = findMotionEnd(id);

        msgBox.setInformativeText("WARNING! Deleting complete motion block!");
        msgBox.setText("Are you sure?");
    } else if(ui->log->item(logMap.value(id),2)->text().contains("Motion end", Qt::CaseSensitive)){
        startId = findMotionStart(id);

        msgBox.setInformativeText("WARNING! Deleting complete motion block!");
        msgBox.setText("Are you sure?");
    }

    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){
        for(int j=startId; j<=endId; j++ ){
            ui->log->removeRow(logMap.value(j));

            QMap<int, int>::iterator i = logMap.find(j);
            while( i != logMap.end() ){
                 i.value() = i.value()-1;
                 ++i;
             }
            logMap.remove(j);
        }
    }
}

void LogWidget::onGoto(int id)
{
    QMessageBox msgBox;
    QString msg = "Start - End\n";
    msg.append(ui->log->item(logMap.value(id),1)->text() + " - " + ui->log->item(logMap.value(findMotionEnd(id)),1)->text());

    msgBox.setText(msg);
    msgBox.exec();
}

int LogWidget::findMotionStart(int endId)
{
    int startId = -1;
    QMap<int, int>::iterator i = logMap.find(endId);
    while( i != logMap.begin() ){
        if(ui->log->item(i.value(),2)->text().contains("Motion start", Qt::CaseSensitive)){
            startId = i.key();
            break;
        }
         --i;
     }
    return startId;
}

int LogWidget::findMotionEnd(int startId)
{
    int endId = -1;
    QMap<int, int>::iterator i = logMap.find(startId);
    while( i != logMap.end() ){
        if(ui->log->item(i.value(),2)->text().contains("Motion end", Qt::CaseSensitive)){
            endId = i.key();
            break;
        }
         ++i;
     }
    return endId;
}

int LogWidget::motionDistance(int startId, int endId)
{
    int distance=0;
    QMap<int, int>::iterator i = logMap.find(startId);
    while( i != logMap.end() ){
        distance++;
        if(i.value() == endId){
            break;
        }
         ++i;
     }
    return distance;
}
