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

    init();

    ui->log->setRowCount(0);
    ui->log->horizontalHeader()->resizeSection(GOTO, 40);
    ui->log->horizontalHeader()->resizeSection(STATUS, 70);
    ui->log->horizontalHeader()->resizeSection(TIME, 60);
    ui->log->horizontalHeader()->resizeSection(TYPE, 100);
    ui->log->horizontalHeader()->setResizeMode(ENTRY, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(DEL, 40);

    ui->log->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ui->log->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

    connect(&app.connectionState, SIGNAL(color(StateColor)), SLOT(stateColor(StateColor)));
}

LogWidget::~LogWidget()
{

}

void LogWidget::init()
{
    inEditing = false;

    application.dataRecorder.clearSamples();

    ui->log->clearContents();
    ui->log->setRowCount(0);

    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->checkButton->setEnabled(false);
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
        createItem("" ,ui->entryLine->text(),"", Text, -1);
    }
}

void LogWidget::createItem(QString status, QString text, QString time, Button button, int at)
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
        ui->log->setItem(row, TYPE, item);
    } else if(button == RecordEnd){
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("Rec End", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, TYPE, item);
    } else if(button == MotionStart){
        txt = text;

        QTableWidgetItem* gotoButton = new QTableWidgetItem(QIcon(":/icons/back-arrow.png"),"", 0);
        gotoButton->setFlags(gotoButton->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, GOTO, gotoButton);

        QTableWidgetItem* item = new QTableWidgetItem("Motion start", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, TYPE, item);
    } else if(button == MotionEnd){
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("Motion end", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, TYPE, item);
    } else {
        txt = text;

        QTableWidgetItem* item = new QTableWidgetItem("", 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, TYPE, item);
    }

    if(button == Insert){
        QTableWidgetItem* timeItem = new QTableWidgetItem(ui->log->item(row-1,TIME)->text(),0);
        timeItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,TIME,timeItem);
    } else if( time != "" ) {
        QTableWidgetItem* timeItem = new QTableWidgetItem(time,0);
        timeItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,TIME,timeItem);
    } else {
        QTableWidgetItem* timeItem = new QTableWidgetItem(QTime::currentTime().toString(),0);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row,TIME,timeItem);
    }

    if( time != "" || button == Insert){
        QTableWidgetItem* item = new QTableWidgetItem(txt,0);
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->log->setItem(row,ENTRY,item);
    } else {
        QTableWidgetItem* item = new QTableWidgetItem(txt,0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row,ENTRY,item);
    }

    if (status == ""){
        QTableWidgetItem* statusIcon = new QTableWidgetItem("", 0);
        statusIcon->setFlags(statusIcon->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, STATUS, statusIcon);
    } else {
        if(status.contains("Unknown", Qt::CaseSensitive)){
            QTableWidgetItem* statusIcon = new QTableWidgetItem(QIcon(":/icons/warning-icon.png"),status, 0);
            statusIcon->setFlags(statusIcon->flags() & ~Qt::ItemIsEditable);
            ui->log->setItem(row, STATUS, statusIcon);
        } else if(status.contains("Ok", Qt::CaseSensitive)){
            QTableWidgetItem* statusIcon = new QTableWidgetItem(QIcon(":/icons/tick-icon.png"),status, 0);
            statusIcon->setFlags(statusIcon->flags() & ~Qt::ItemIsEditable);
            ui->log->setItem(row, STATUS, statusIcon);
        } else {
            QTableWidgetItem* statusIcon = new QTableWidgetItem(QIcon(":/icons/delete-icon.png"),status, 0);
            statusIcon->setFlags(statusIcon->flags() & ~Qt::ItemIsEditable);
            ui->log->setItem(row, STATUS, statusIcon);
        }
    }

    if(button != RecordStart && button != RecordEnd){
        QTableWidgetItem* del = new QTableWidgetItem(QIcon(":/icons/Delete.png"),"", 0);
        del->setFlags(del->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, DEL, del);
    } else {
        QTableWidgetItem* del = new QTableWidgetItem("", 0);
        del->setFlags(del->flags() & ~Qt::ItemIsEditable);
        ui->log->setItem(row, DEL, del);
    }

    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidget::on_recStartButton_clicked()
{
    init();

    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem("" ,msg, "", RecordStart, -1);
    ui->recStartButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
    ui->checkButton->setEnabled(false);
    ui->clearButton->setEnabled(false);
    ui->loadButton->setEnabled(false);

    ui->entryLine->setFocus();

    connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)),
            &application.dataRecorder,   SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);
}

void LogWidget::on_recEndButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to finish recording?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok){

        disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)),
                   &application.dataRecorder,   SLOT(onReceiveMessage(ActiveMessage)));
        QString msg;
        if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
        createItem("", msg, "", RecordEnd, -1);
        ui->recStartButton->setEnabled(false);
        ui->recEndButton->setEnabled(false);
        ui->motionStartButton->setEnabled(false);
        ui->entryLine->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->checkButton->setEnabled(true);
        ui->clearButton->setEnabled(true);

        for(int i=0; i<ui->log->rowCount(); i++){
            ui->log->item(i, ENTRY)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }

        //disconnect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    }

    ui->entryLine->setFocus();
}

void LogWidget::on_motionStartButton_clicked()
{
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem("Unknown", msg, "", MotionStart, -1);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);
    ui->recEndButton->setEnabled(false);

    ui->entryLine->setFocus();
}

void LogWidget::on_motionEndButton_clicked()
{
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItem("", msg, "", MotionEnd, -1);
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
        ui->checkButton->setEnabled(true);
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
    }
}

void LogWidget::on_checkButton_clicked()
{
    int failed = 0;

    QTime tooShort(0,0,1,0);
    QTime motionStart, motionEnd;
    QTime recordStart = QTime::fromString(ui->log->item(findRecordStart(),TIME)->text(), "hh:mm:ss");
    QTime recordEnd = QTime::fromString(ui->log->item(findRecordEnd(),TIME)->text(), "hh:mm:ss");

    for(int i = 0; i < ui->log->rowCount(); i++){
        if(isMotionStart(i)){
            motionStart = QTime::fromString(ui->log->item(i,TIME)->text(), "hh:mm:ss");
            qDebug() << "Motion start: " << ui->log->item(i, TIME)->text() << " - " << ui->log->item(i, ENTRY)->text() << " - Row: " << i;
        } else if(isMotionEnd(i)){
            motionEnd = QTime::fromString(ui->log->item(i,TIME)->text(), "hh:mm:ss");

            QTime motStartToMotEnd = QTime(0,0,0,0).addMSecs(motionStart.msecsTo(motionEnd));

            if(motStartToMotEnd > tooShort){
                QTime motStartToRecStart = QTime(0,0,0,0).addMSecs(recordStart.msecsTo(motionStart));
                QTime motEndToRecStart = QTime(0,0,0,0).addMSecs(recordStart.msecsTo(motionEnd));
                QTime recEndToRecStart = QTime(0,0,0,0).addMSecs(recordStart.msecsTo(recordEnd));
                qDebug() << motStartToRecStart.toString();
                qDebug() << motEndToRecStart.toString();
                qDebug() << recEndToRecStart.toString();
            } else {
                ui->log->item(findMotionStart(i), STATUS)->setText("Failed");
                ui->log->item(findMotionStart(i), STATUS)->setIcon(QIcon(":/icons/delete-icon.png"));
                failed++;
            }
        }
    }

    qDebug() << "Failed count: " << failed;

}

void LogWidget::onDelRow(int row)
{
    int startRow = row;
    int endRow = row;

    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to delete this row?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    if(ui->log->item(row,TYPE)->text().contains("Motion start", Qt::CaseSensitive)){
        endRow = findMotionEnd(row);

        msgBox.setText("WARNING! Deleting complete motion block!");
        msgBox.setInformativeText("Are you sure?");
        msgBox.setIcon(QMessageBox::Warning);
    } else if(ui->log->item(row,TYPE)->text().contains("Motion end", Qt::CaseSensitive)){
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
        msg.append(ui->log->item(row,TIME)->text() + " - " + ui->log->item(findMotionEnd(row),TIME)->text());

    msgBox.setText(msg);
    msgBox.exec();
}

int LogWidget::findMotionStart(int endRow)
{
    int startRow = -1;

    for(int i = endRow; i > 0; i--){
        if(ui->log->item(i,TYPE)->text().contains("Motion start", Qt::CaseSensitive)){
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
        if(ui->log->item(i,TYPE)->text().contains("Motion end", Qt::CaseSensitive)){
            endRow = i;
            break;
        }
    }

    return endRow;
}

int LogWidget::findRecordStart()
{
    int startRow = -1;

    for(int i = 0; i < ui->log->rowCount(); i++){
        if(ui->log->item(i,TYPE)->text().contains("Rec Start", Qt::CaseSensitive)){
            startRow = i;
            break;
        }
    }

    return startRow;
}

int LogWidget::findRecordEnd()
{
    int endRow = -1;

    for(int i = ui->log->rowCount()-1; i > 0; i--){
        if(ui->log->item(i,TYPE)->text().contains("Rec End", Qt::CaseSensitive)){
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

    if(row != -1 && row != ui->log->rowCount()-1){
        QAction* selectedItem = myMenu.exec(globalPos);
        if (selectedItem)
        {
            createItem("" ,ui->entryLine->text(), "", Insert, row+1);

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
    if(column == GOTO){
        if(row == findMotionStart(row)) onGoto(row);
    } else if(column == DEL){
        onDelRow(row);
    }
}

void LogWidget::on_log_cellChanged(int row, int column)
{
//    int row = item->row();
//    int column = item->column();
    QTableWidgetItem* item = ui->log->item(row, column);
    if(column == TIME && item->isSelected() && row < ui->log->rowCount()){
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
            ui->log->closePersistentEditor( ui->log->item(row, column));
            inEditing = false;
        }
    }

    if(column == TIME && (isMotionStart(row) || isMotionEnd(row)) && item->isSelected() ){
        ui->log->item(findMotionStart(row), STATUS)->setText("Unknown");
        ui->log->item(findMotionStart(row), STATUS)->setIcon(QIcon(":/icons/warning-icon.png"));
    }

    if(column == GOTO && row==0) qDebug() << "itemchanged";
}

bool LogWidget::isRecordEnd(int row)
{
    return ui->log->item(row,TYPE)->text().contains("Rec End", Qt::CaseSensitive);
}

bool LogWidget::isRecordStart(int row)
{
    return ui->log->item(row,TYPE)->text().contains("Rec Start", Qt::CaseSensitive);
}

bool LogWidget::isMotionStart(int row)
{
    return ui->log->item(row,TYPE)->text().contains("Motion start", Qt::CaseSensitive);
}

bool LogWidget::isMotionEnd(int row)
{
    return ui->log->item(row,TYPE)->text().contains("Motion end", Qt::CaseSensitive);
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

    ts << "#Status,Time,Type,Entry" << endl;
    for (int i=0; i<ui->log->rowCount(); i++){
      ts << ui->log->item(i,STATUS)->text() << "," << ui->log->item(i,TIME)->text() << "," << ui->log->item(i,TYPE)->text() << "," << ui->log->item(i,ENTRY)->text() << endl;
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
            while ( !line.isEmpty() && line != "#Status,Time,Type,Entry" ){
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
        QString status = csvIterator.next();
        QString time   = csvIterator.next();
        QString type   = csvIterator.next();
        QString text   = csvIterator.next();

        if(type.contains("Motion start", Qt::CaseSensitive)){
            createItem(status, text, time, MotionStart , -1);
        } else if(type.contains("Motion end", Qt::CaseSensitive)){
            createItem(status, text, time, MotionEnd , -1);
        } else if(type.contains("Rec Start", Qt::CaseSensitive)){
            createItem(status, text, time, RecordStart , -1);
        } else if(type.contains("Rec End", Qt::CaseSensitive)){
            createItem(status, text, time, RecordEnd , -1);
            ui->entryLine->setEnabled(false);
        } else {
            createItem(status, text, time, Text, -1);
        }

    }
}

void LogWidget::stateColor(StateColor color) {

    QString col;

    if (color == RED) {

        col = "RED";
    }
    else if (color == YELLOW) {

        col = "YELLOW";
    }
    else if (color == GREEN) {

        col = "GREEN";
    }
    else {

        col = "unknown";
    }

    qDebug() << "Signal is " << col;
}
