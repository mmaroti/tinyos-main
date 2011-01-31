/** Copyright (c) 2010, 2011, University of Szeged
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
#include <QValidator>
#include <QRegExp>
#include "LogDialog.h"
#include <QFile>
#include <QFileDialog>

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
    ui->log->horizontalHeader()->resizeSection(1, 60);
    ui->log->horizontalHeader()->resizeSection(2, 100);
    ui->log->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(4, 40);

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
    inEditing = false;

    ui->log->clearContents();
    ui->log->setRowCount(0);

    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->clearButton->setEnabled(false);
    ui->loadButton->setEnabled(true);
    ui->recStartButton->setEnabled(true);
    ui->entryLine->setEnabled(true);

    //ui->saveButton->setStyleSheet("* { background-color: rgb(255,185,185) }");

    ui->entryLine->setFocus();
}


void LogWidget::on_entryLine_returnPressed()
{
    if(!ui->entryLine->text().isNull()){
        createItem(ui->entryLine->text(),"", Text, -1);
    }
}

void LogWidget::createItem(QString text, QString time, Button button, int at)
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

        QTableWidgetItem* item = new QTableWidgetItem("Rec Start", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 2, item);
    } else if(button == RecordEnd){
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("Rec End", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 2, item);
    } else if(button == MotionStart){
        txt = text;

        QTableWidgetItem* gotoButton = new QTableWidgetItem(QIcon(":/icons/back-arrow.png"),"", 0);
        gotoButton->setFlags(gotoButton->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 0, gotoButton);

        QTableWidgetItem* item = new QTableWidgetItem("Motion start", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 2, item);
    } else if(button == MotionEnd){
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("Motion end", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 2, item);
    } else {
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 2, item);
    }

    if(button == Insert){
        QTableWidgetItem* timeItem = new QTableWidgetItem(ui->log->item(row-1,1)->text(),0);
        timeItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,1,timeItem);
    } else if( time != "" ) {
        QTableWidgetItem* timeItem = new QTableWidgetItem(time,0);
        timeItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,1,timeItem);
    } else {
        QTableWidgetItem* timeItem = new QTableWidgetItem(QTime::currentTime().toString(),0);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row,1,timeItem);
    }

    if( time != "" || button == Insert){
        QTableWidgetItem* item = new QTableWidgetItem(txt,0);
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,3,item);
    } else {
        QTableWidgetItem* item = new QTableWidgetItem(txt,0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row,3,item);
    }

    if(button != RecordStart && button != RecordEnd){
        QTableWidgetItem* del = new QTableWidgetItem(QIcon(":/icons/Delete.png"),"", 0);
        del->setFlags(del->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 4, del);
    } else {
        QTableWidgetItem* del = new QTableWidgetItem("", 0);
        del->setFlags(del->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, 4, del);
    }

    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidget::on_recStartButton_clicked()
{
    init();

    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg, "", RecordStart, -1);
    ui->recStartButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
    ui->clearButton->setEnabled(false);
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
        QString msg;
        if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
        createItem(msg, "", RecordEnd, -1);
        ui->recStartButton->setEnabled(false);
        ui->recEndButton->setEnabled(false);
        ui->motionStartButton->setEnabled(false);
        ui->entryLine->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->clearButton->setEnabled(true);

        for(int i=0; i<ui->log->rowCount(); i++){
            ui->log->item(i, 3)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }

        //disconnect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    }

    ui->entryLine->setFocus();
}

void LogWidget::on_motionStartButton_clicked()
{
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg, "", MotionStart, -1);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);
    ui->recEndButton->setEnabled(false);

    ui->entryLine->setFocus();
}

void LogWidget::on_motionEndButton_clicked()
{
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem(msg, "", MotionEnd, -1);
    ui->motionEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->recEndButton->setEnabled(true);

    ui->entryLine->setFocus();
    ui->log->update();
}

void LogWidget::on_loadButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select a file to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        init();

        ui->recStartButton->setEnabled(false);
        ui->recEndButton->setEnabled(false);
        ui->motionStartButton->setEnabled(false);
        ui->motionEndButton->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->clearButton->setEnabled(true);

        disconnect(ui->entryLine, SIGNAL(returnPressed()), this, SLOT(on_entryLine_returnPressed()));

        loadLog(file);
        if(ui->log->rowCount() == 0) ui->recStartButton->setEnabled(true);
    }
}

void LogWidget::on_saveButton_clicked()
{
    //connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        ui->recStartButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->entryLine->setEnabled(true);
        connect(ui->entryLine, SIGNAL(returnPressed()), this, SLOT(on_entryLine_returnPressed()));

        saveLog( fn );
        //ui->saveButton->setStyleSheet("* { background-color: rgb(185,255,185) }");
    }

    ui->entryLine->setFocus();
}

void LogWidget::on_clearButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setText("WARNING! Deleting ALL log and sample data!");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setIcon(QMessageBox::Warning);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Ok){
        init();

        application.dataRecorder.clearSamples();
    }
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

bool LogWidget::isMotionStart(int row)
{
    return ui->log->item(row,2)->text().contains("Motion start", Qt::CaseSensitive);
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

    if(row != -1 && row != ui->log->rowCount()-1){
        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem)
        {
            createItem(ui->entryLine->text(), "", Insert, row+1);

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
    } else if(column == 4){
        onDelRow(row);
    }
}

void LogWidget::on_log_cellChanged(int row, int column)
{
//    int row = item->row();
//    int column = item->column();
    QTableWidgetItem* item = ui->log->item(row, column);
    if(column == 1 && item->isSelected() && row < ui->log->rowCount()){
        QTime before = QTime::fromString(ui->log->item(row-1,column)->text(), "hh:mm:ss");
        QTime after = QTime::fromString(ui->log->item(row+1,column)->text(), "hh:mm:ss");
        QTime now = QTime::fromString(ui->log->item(row,column)->text(), "hh:mm:ss");

        if( (!now.isValid() || before > now || after < now) && !inEditing ){
            QMessageBox msgBox;
            msgBox.setText("Time value invalid!\nPlease enter a time value between\n"+ui->log->item(row-1,column)->text()+" - "+ui->log->item(row+1,column)->text());
            msgBox.exec();

            ui->log->item(row,column)->setText( ui->log->item(row-1,column)->text() );
            ui->log->openPersistentEditor( ui->log->item(row, column));
            return;

//            inEditing = true;
//
//            QTableWidgetItem *item = ui->log->item(row,column);
//            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
//            ui->log->editItem(item);

        } else {
            inEditing = false;
        }
    }
    if(column == 0 && row==0) qDebug() << "itemchanged";
}

bool LogWidget::isRecordEnd(int row)
{
    return ui->log->item(row,2)->text().contains("Rec End", Qt::CaseSensitive);
}

void LogWidget::saveLog(const QString &filename)
{
    application.dataRecorder.saveSamples(filename);

    QFile f( filename );

    if( !f.open( QIODevice::Append ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#LOG metadata" << endl;

    ts << "#Time,Type,Entry" << endl;
    for (int i=0; i<ui->log->rowCount(); i++){
      ts << ui->log->item(i,1)->text() << "," << ui->log->item(i,2)->text() << "," << ui->log->item(i,3)->text() << endl;
    }

    ts.flush();
    f.close();
}

void LogWidget::loadLog(const QString &filename)
{
    application.dataRecorder.loadSamples(filename);
    application.dataRecorder.loadCalibFromFile(filename);

    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine(); //skip first line of csv

        if(line[0] != QChar('#')){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            line = ts.readLine();
            while ( !line.isEmpty() && line != "#Time,Type,Entry" ){
                line = ts.readLine();         // line of text excluding '\n'
            }
            line = ts.readLine();
            while( !line.isEmpty() ){
                csvToLog(line);            //convert line string to sample
                line = ts.readLine();
            }
            f.close();
        }
    }

}

void LogWidget::csvToLog(const QString &line)
{
    QStringList list = line.split(",");
    QStringListIterator csvIterator(list);

    if(csvIterator.hasNext()){
        QString time = csvIterator.next();
        QString type = csvIterator.next();
        QString text = csvIterator.next();

        if(type.contains("Motion start", Qt::CaseSensitive)){
            createItem(text, time, MotionStart , -1);
        } else if(type.contains("Motion end", Qt::CaseSensitive)){
            createItem(text, time, MotionEnd , -1);
        } else if(type.contains("Rec Start", Qt::CaseSensitive)){
            createItem(text, time, RecordStart , -1);
        } else if(type.contains("Rec End", Qt::CaseSensitive)){
            createItem(text, time, RecordEnd , -1);
            ui->entryLine->setEnabled(false);
        } else {
            createItem(text, time, Text, -1);
        }

    }
}
