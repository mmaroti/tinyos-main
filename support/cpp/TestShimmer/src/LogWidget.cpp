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
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QValidator>
#include <QRegExp>
#include <QFile>
#include <QFileDialog>

namespace {

    const int NO_MORE = -1;
    const int TOO_SHORT_IN_SEC = 1;

    const char PASSED_TEXT[] = "Passed";
    const char FAILED_TEXT[] = "Failed";
}

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app),
        blockingBox(0)
{
    ui->setupUi(this);

    init();

    ui->log->setRowCount(0);
    ui->log->horizontalHeader()->resizeSection(GOTO, 40);
    ui->log->horizontalHeader()->resizeSection(STATUS, 100);
    ui->log->horizontalHeader()->resizeSection(TIME, 60);
    ui->log->horizontalHeader()->resizeSection(TYPE, 100);
    ui->log->horizontalHeader()->setResizeMode(ENTRY, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(DEL, 40);

    ui->log->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ui->log->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->iconLabel->setTextFormat(Qt::RichText);

    ui->iconLabel->setText("<img src=\":/icons/NoConnection.png\">");

    connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

    connect(&app.connectionState, SIGNAL(color(StateColor)), SLOT(stateColor(StateColor)));

    blockingBox = new QMessageBox(QMessageBox::Information,
                                  "Checking record",
                                  "Please wait, computing...",
                                  QMessageBox::NoButton, this);
    blockingBox->setModal(true);
    blockingBox->setStandardButtons(QMessageBox::NoButton);
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

    entryLineInit();
}

void LogWidget::entryLineInit()
{
    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidget::createItem(QString txt, int row, Column column, bool editable)
{
    QTableWidgetItem* item = new QTableWidgetItem(txt, 0);
    if(editable) {
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    ui->log->setItem(row, column, item);
}

void LogWidget::createItem(QString txt, int row, Column column, bool editable, QIcon icon)
{
    QTableWidgetItem* item = new QTableWidgetItem(icon, txt, 0);
    if(editable) {
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    ui->log->setItem(row, column, item);
}

void LogWidget::createItems(int at, Mode mode, Type type, Status status)
{
    int row = createRow(at);

    createGoto(row, type);
    createStatus(row, status);
    createType(row, type);
    if(mode == INSERT){
        createTime(row, INSERT);
    } else {
        createTime(row);
    }
    createEntry(row, mode, type);
    createDel(row, type);
}

void LogWidget::createItems(int at, Type type, TimeMode timeMode, Status status, QString txt, QString time)
{
    int row = createRow(at);

    createGoto(row, type);
    createStatus(row, status);
    createType(row, type);
    createTime(row, time, timeMode);
    createEntry(row, txt);
    createDel(row, type);
}

int LogWidget::createRow(int at)
{
    if(at == -1){
        at = ui->log->rowCount();
        ui->log->insertRow(at);
    } else {
        ui->log->insertRow(at);
    }

    return at;
}

void LogWidget::createGoto(int row, Type type)
{
    if( type == MOTIONSTART){
        createItem("", row, GOTO, false, QIcon(":/icons/back-arrow.png"));
    } else {
        createItem("", row, GOTO, false);
    }
}

void LogWidget::createStatus(int row, Status status)
{
    if (status == EMPTY){
        createItem("", row, STATUS, false);
    } else {
        if(status == UNKNOWN){
            createItem("Unknown", row, STATUS, false, QIcon(":/icons/warning-icon.png"));
        } else if(status == OK){
            createItem("OK", row, STATUS, false, QIcon(":/icons/tick-icon.png"));
        } else if(status == FAILED){
            createItem("Failed", row, STATUS, false, QIcon(":/icons/delete-icon.png"));
        }
    }
}

void LogWidget::createType(int row, Type type)
{
    if(type == RECORDSTART){
        createItem("Record Start", row, TYPE, false);
    } else if(type == RECORDEND){
        createItem("Record End", row, TYPE, false);
    } else if(type == MOTIONSTART){
        createItem("Motion Start", row, TYPE, false);
    } else if(type == MOTIONEND){
        createItem("Motion End", row, TYPE, false);
    } else if(type == TEXT) {
        createItem("", row, TYPE, false);
    }
}

void LogWidget::createTime(int row, Mode) // FIXME Duplication, a bool with a default false would suffice
{
    createItem(ui->log->item(row-1,TIME)->text(), row, TIME, true);
}

void LogWidget::createTime(int row)
{
    createItem(QTime::currentTime().toString(), row, TIME, false);
}

void LogWidget::createTime(int row, QString time, TimeMode timeMode)
{
    if( timeMode == SET ) {
        createItem(time, row, TIME, true);
    } else if( timeMode == SETNOTEDITABLE ){
        createItem(time, row, TIME, false);
    }
}

void LogWidget::createEntry(int row, Mode mode, Type type)
{
    if( mode == NORMAL ){
        if( type == RECORDSTART){
            createItem(QDate::currentDate().toString()+" - "+ui->entryLine->text(), row, ENTRY, false);
        } else {
            createItem(ui->entryLine->text(), row, ENTRY, true);
        }
    }
}

void LogWidget::createEntry(int row, QString txt)
{
    createItem(txt, row, ENTRY, true);
}

void LogWidget::createDel(int row, Type type)
{
    if(type != RECORDSTART && type != RECORDEND){
        createItem("", row, DEL, false, QIcon(":/icons/Delete.png"));
    } else {
        createItem("", row, DEL, false);
    }
}

void LogWidget::on_entryLine_returnPressed()
{
    if(!ui->entryLine->text().isNull()){
        createItems(-1, NORMAL, TEXT, EMPTY);
    }

    entryLineInit();
}

void LogWidget::on_recStartButton_clicked()
{
    if(application.connectionState.isConnected()){
        init();

        createItems(-1, NORMAL, RECORDSTART, EMPTY);

        ui->recStartButton->setEnabled(false);
        ui->motionStartButton->setEnabled(true);
        ui->saveButton->setEnabled(false);
        ui->checkButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->loadButton->setEnabled(false);

        entryLineInit();

        connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)),
                &application.dataRecorder,   SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);
    } else {
        QMessageBox msgBox;
        msgBox.setText("Cannot start recording, while system is not connected!");
        msgBox.exec();
    }
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

        createItems(-1, NORMAL, RECORDEND, EMPTY);

        //createItem("", msg, "", RecordEnd, false, -1);
        ui->recStartButton->setEnabled(false);
        ui->recEndButton->setEnabled(false);
        ui->motionStartButton->setEnabled(false);
        ui->entryLine->setEnabled(false);
        ui->saveButton->setEnabled(true);
        ui->checkButton->setEnabled(true);
        ui->clearButton->setEnabled(true);

        for(int i=0; i<ui->log->rowCount(); i++){ // Crashes is loop, most likely null pointer
            ui->log->item(i, TIME)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->log->item(i, ENTRY)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }

        //disconnect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    }

    entryLineInit();
}

void LogWidget::on_motionStartButton_clicked()
{
    qDebug() << "Mot Start";
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItems(-1, NORMAL, MOTIONSTART, UNKNOWN);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);
    ui->recEndButton->setEnabled(false);

    entryLineInit();
}

void LogWidget::on_motionEndButton_clicked()
{
    qDebug() << "Mot end";
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItems(-1, NORMAL, MOTIONEND, EMPTY);
    ui->motionEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->recEndButton->setEnabled(true);

    entryLineInit();

    ui->log->update();
}

void LogWidget::on_loadButton_clicked()
{
    qDebug() << "Load";
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
        ui->entryLine->setEnabled(false);

        disconnect(ui->entryLine, SIGNAL(returnPressed()), this, SLOT(on_entryLine_returnPressed()));

        loadLog(file);
        if(ui->log->rowCount() == 0) ui->recStartButton->setEnabled(true);
    }
}

void LogWidget::on_saveButton_clicked()
{
    qDebug() << "Save";
    //connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        ui->recStartButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        //ui->entryLine->setEnabled(true);
        //connect(ui->entryLine, SIGNAL(returnPressed()), this, SLOT(on_entryLine_returnPressed()));

        saveLog( fn );
    }

    entryLineInit();
}

void LogWidget::on_clearButton_clicked()
{
    qDebug() << "Clears";
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
    qDebug() << "Check";
    startChecking();
}

void LogWidget::onDelRow(int row)
{
    qDebug() << "Del row: " << row;
    int startRow = row;
    int endRow = row;

    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to delete this row?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    if(isMotionStart(row)){
        endRow = findMotionEnd(row);

        msgBox.setText("WARNING! Deleting complete motion block!");
        msgBox.setInformativeText("Are you sure?");
        msgBox.setIcon(QMessageBox::Warning);
    } else if(isMotionEnd(row)){
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

    entryLineInit();
}

void LogWidget::onGoto(int row)
{
    qDebug() << "Goto: " << row;
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
        if(ui->log->item(i,TYPE)->text().contains("Motion Start", Qt::CaseSensitive)){
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
        if(ui->log->item(i,TYPE)->text().contains("Motion End", Qt::CaseSensitive)){
            endRow = i;
            break;
        }
    }

    return endRow;
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
            qDebug() << "Insert";
            createItems(row+1, INSERT, TEXT, EMPTY);

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
        if(isMotionStart(row)) onGoto(row);
    } else if(column == DEL){
        onDelRow(row);
    }
}

void LogWidget::on_log_cellChanged(int row, int column)
{
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

bool LogWidget::isRecordEnd(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Record End", Qt::CaseSensitive);
}

bool LogWidget::isRecordStart(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Record Start", Qt::CaseSensitive);
}

bool LogWidget::isMotionStart(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Motion Start", Qt::CaseSensitive);
}

bool LogWidget::isMotionEnd(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Motion End", Qt::CaseSensitive);
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
    for (int i=0; i<ui->log->rowCount(); i++){ // FIXME Crashes in the loop: item is null pointer
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

        if(type.contains("Motion Start", Qt::CaseSensitive)){

            if(status.contains("Unknown", Qt::CaseSensitive)){
                createItems(-1, MOTIONSTART, SET, UNKNOWN, text, time);
            } else if(status.contains("Passed", Qt::CaseSensitive)){
                createItems(-1, MOTIONSTART, SET, OK, text, time);
            } else {
                createItems(-1, MOTIONSTART, SET, FAILED, text, time);
            }

        } else if(type.contains("Motion End", Qt::CaseSensitive)){
            createItems(-1, MOTIONEND, SET, EMPTY, text, time);
        } else if(type.contains("Record Start", Qt::CaseSensitive)){
            createItems(-1, RECORDSTART, SETNOTEDITABLE, EMPTY, text, time);
        } else if(type.contains("Record End", Qt::CaseSensitive)){
            createItems(-1, RECORDEND, SETNOTEDITABLE, EMPTY, text, time);

            ui->entryLine->setEnabled(false);
        } else {
            createItems(-1, TEXT, SET, EMPTY, text, time);
        }

    }
}

void LogWidget::stateColor(StateColor color) {

    QString col;

    if (color == RED) {

        col = "RED";
        ui->iconLabel->setText("<img src=\":/icons/NoConnection.png\">");
    }
    else if (color == YELLOW) {

        col = "YELLOW";
        ui->iconLabel->setText("<img src=\":/icons/warning-icon.png\">");
    }
    else if (color == GREEN) {

        col = "GREEN";
        ui->iconLabel->setText("<img src=\":/icons/Connection.png\">");
    }
    else {

        col = "unknown";
    }

    qDebug() << "Signal is " << col;
}

//=============================================================================

void LogWidget::startChecking() {

    startAt = endAt = -1;

    writeToConsole(" checking started");

    checkNextMotion();
}

int LogWidget::findNextMot() {

    startAt = findMotStart(endAt+1);

    if (startAt == NO_MORE) {

        return NO_MORE;
    }

    endAt = findMotEnd(startAt+1);

    Q_ASSERT(endAt != NO_MORE);

    return startAt;
}

void LogWidget::checkNextMotion() {

    if (findNextMot() == NO_MORE) {

        finishedChecking();
    }
    else if (isMotionTooShort()) {

        markAsFailed();

        checkNextMotion();
    }
    else if (isAlreadyPassed(startAt)) {

        checkNextMotion();
    }
    else {

        startSolver();
    }
}

void LogWidget::startSolver() {

    int begin = recStart().secsTo(motionStart());

    int end = recStart().secsTo(motionEnd());

    bool error = application.mockSolver.start(begin, end, recLengthInSec());

    if (error) {

        finishedChecking();
    }
    else {

        blockingBox->show();
    }
}

void LogWidget::solverFinished(bool error, const QString message) {

    if (error) {

        qDebug() << "Solver failed at row " << startAt;

        writeToConsole( atRow() + message );

        markAsFailed();
    }
    else {

        motionOK();
    }

    checkNextMotion();
}

void LogWidget::finishedChecking() {

    blockingBox->hide();
}

void LogWidget::markAsFailed() {

    setText(startAt, STATUS, FAILED_TEXT);

    setIcon(startAt, STATUS, QIcon(":/icons/delete-icon.png"));
}

void LogWidget::motionOK() {

    setText(startAt, STATUS, PASSED_TEXT);

    setIcon(startAt, STATUS, QIcon(":/icons/tick-icon.png"));
}

void LogWidget::setText(int row, Column col, const char text[]) {

    item(row, col).setText(text);
}

void LogWidget::setIcon(int row, Column col, const QIcon& icon) {

    item(row, col).setIcon(icon);
}

const QTime LogWidget::motionStart() const {

    Q_ASSERT(isMotionStart(startAt));

    return timeInRow(startAt);
}

const QTime LogWidget::motionEnd() const {

    Q_ASSERT(isMotionEnd(endAt));

    return timeInRow(endAt);
}

bool LogWidget::isMotionTooShort() const {

    return motionStart().secsTo(motionEnd()) <= TOO_SHORT_IN_SEC;
}

int LogWidget::findMotStart(int pos) const {

    for ( ; pos < rowCount(); ++pos ) {

        if( isMotionStart(pos) ) {

            return pos;
        }
    }

    return NO_MORE;
}

int LogWidget::findMotEnd(int pos) const {

    for ( ; pos < rowCount(); ++pos ) {

        if( isMotionEnd(pos) ) {

            return pos;
        }
    }

    return NO_MORE;
}

const QTableWidget& LogWidget::tableWidget() const {

    return *(ui->log);
}

const QTableWidgetItem& LogWidget::item(int row, Column col) const {

    return *(tableWidget().item(row, col));
}

QTableWidgetItem& LogWidget::item(int row, Column col) {

    return *(ui->log->item(row, col));
}

const QTime LogWidget::timeInRow(int row) const {

    return QTime::fromString(item(row,TIME).text(), "hh:mm:ss"); // FIXME Why isn't it stored as data???
}

int LogWidget::rowCount() const {

    return tableWidget().rowCount();
}

bool LogWidget::isAlreadyPassed(const int row) const {

    return item(row, STATUS).text().contains(PASSED_TEXT, Qt::CaseSensitive);
}

int LogWidget::recLengthInSec() const {

    const int lastRow = rowCount() - 1;

    Q_ASSERT(lastRow > 0);

    Q_ASSERT(isRecordEnd(lastRow));

    const QTime recEnd = timeInRow(lastRow);

    int recLength = recStart().secsTo(recEnd);

    Q_ASSERT(recLength >= 0);

    return recLength;
}

const QTime LogWidget::recStart() const {

    Q_ASSERT(rowCount() > 2);

    Q_ASSERT(isRecordStart(0));

    return timeInRow(0);
}

const QString LogWidget::atRow() const {

    return QString(" - row " + QString::number(startAt) + " ");
}

void LogWidget::writeToConsole(const QString& msg) const {

    QString time = QDateTime::currentDateTime().time().toString();

    application.showConsoleMessage( time + msg );
}

void LogWidget::on_log_cellDoubleClicked(int row, int column)
{
    if(column == STATUS && isAlreadyPassed(row)){

        int end = findMotEnd(row+1);

        Q_ASSERT(end!=NO_MORE);

        qDebug() << "Start " << row << ", End " << end;
    }
}
