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

#include "LogWidgetBasic.h"
#include "ui_LogWidgetBasic.h"
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
#include <QTextStream>
#include <QFontMetrics>
#include "DataHolder.hpp"
#include "GLWindow.hpp"
#include "SQLDialog.hpp"
#include "RecordHandler.hpp"

namespace {

    const int NO_MORE = -1;
    const int TOO_SHORT_IN_SEC = 1;
    const qint64 INVALID_RECORD_ID = -1;

    const char DATE_FORMAT[] = "yyyy-MM-dd";

    const char PASSED_TEXT[] = "Play";
    const char FAILED_TEXT[] = "Failed";
    const char UNKNOWN_TEXT[] ="Unknown";

    const char NOT_CONNECTED[] = "<font color=\"red\">Not connected</font>";
    const char MSG_LOSS[]      = "<font color=\"red\">Packet loss</font>";
    const char CONNECTION_OK[] = "<font color=\"green\">Connection OK</font>";
}

LogWidgetBasic::LogWidgetBasic(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidgetBasic),
        application(app),
        blockingBox(0),
        dial(new SQLDialog),
        recSelect(new RecordHandler),
        recordID(INVALID_RECORD_ID),
        extrema(new double[SIZE_OF_ARRAY])
{
    ui->setupUi(this);

    init();

    QFont defaultBoldFont = QFont();
    defaultBoldFont.setBold(true);


    QFontMetrics fontMetrics(ui->log->font());

    ui->log->setRowCount(0);
    ui->log->horizontalHeader()->resizeSection(STATUS, fontMetrics.width(UNKNOWN_TEXT)+40);
    ui->log->horizontalHeader()->resizeSection(TIME, fontMetrics.width("hh:mm:ss")+20);
    ui->log->horizontalHeader()->resizeSection(TYPE, fontMetrics.width("Record Start")+20);
    ui->log->horizontalHeader()->setResizeMode(ENTRY, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(DEL, 40);

    ui->log->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ui->log->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

    connect(&app.connectionState, SIGNAL(color(StateColor)), SLOT(stateColor(StateColor)));

    blockingBox = new QMessageBox(QMessageBox::Information,
                                  "Checking record",
                                  "Please wait, computing...",
                                  QMessageBox::NoButton, this);
    blockingBox->setModal(true);
    blockingBox->setStandardButtons(QMessageBox::NoButton);

    dial->setWindowTitle("Add or select a person");

    recSelect->setWindowTitle("Select a record");

    connect(dial, SIGNAL(personSelected(Person)), SLOT(onPersonSelected(Person)));

    connect(recSelect, SIGNAL(recordSelected(qint64,Person)), SLOT(onRecordSelected(qint64,Person)));
}

LogWidgetBasic::~LogWidgetBasic()
{
    delete[] extrema;
}

void LogWidgetBasic::init()
{
    inEditing = false;

    application.dataRecorder.clearSamples();

    ui->log->clearContents();
    ui->log->setRowCount(0);


    ui->recStartButton->setEnabled(true);
    ui->recEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->clearButton->setEnabled(true);

    ui->entryLine->setEnabled(true);
    ui->entryLine->setFocus();

    markUnsaved();
}

void LogWidgetBasic::entryLineInit()
{
    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidgetBasic::markUnsaved() {

    isSaved = false;
    ui->saveButton->setStyleSheet("* { background-color: rgb(255,185,185) }");
}

void LogWidgetBasic::markSaved() {

    isSaved = true;
    ui->saveButton->setStyleSheet("* { background-color: rgb(185,255,185) }");
}

void LogWidgetBasic::createItem(QString txt, int row, Column column, bool editable)
{
    QTableWidgetItem* item = new QTableWidgetItem(txt, 0);
    if(editable) {
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    ui->log->setItem(row, column, item);
}

void LogWidgetBasic::createItem(QString txt, int row, Column column, bool editable, QIcon icon)
{
    QTableWidgetItem* item = new QTableWidgetItem(icon, txt, 0);
    if(editable) {
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    ui->log->setItem(row, column, item);
}

void LogWidgetBasic::createItems(int at, Mode mode, Type type, Status status)
{
    int row = createRow(at);

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

void LogWidgetBasic::createItems(int at, Type type, TimeMode timeMode, Status status, QString txt, QString time)
{
    int row = createRow(at);

    createStatus(row, status);
    createType(row, type);
    createTime(row, time, timeMode);
    createEntry(row, txt);
    createDel(row, type);
}

int LogWidgetBasic::createRow(int at)
{
    if(at == -1){
        at = ui->log->rowCount();
        ui->log->insertRow(at);
    } else {
        ui->log->insertRow(at);
    }

    return at;
}

void LogWidgetBasic::createStatus(int row, Status status)
{
    if (status == EMPTY){
        createItem("", row, STATUS, false);
    } else {
        if(status == UNKNOWN){
            createItem(UNKNOWN_TEXT, row, STATUS, false, QIcon(":/icons/Warning.png"));
        } else if(status == OK){
            createItem(PASSED_TEXT, row, STATUS, false, QIcon(":/icons/passed.png"));
        } else if(status == FAILED){
            createItem(FAILED_TEXT, row, STATUS, false, QIcon(":/icons/failed.png"));
        }
    }
}

void LogWidgetBasic::createType(int row, Type type)
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

void LogWidgetBasic::createTime(int row, Mode) // FIXME Duplication, a bool with a default false would suffice
{
    createItem(ui->log->item(row-1,TIME)->text(), row, TIME, true);
}

void LogWidgetBasic::createTime(int row)
{
    createItem(QTime::currentTime().toString(), row, TIME, false);
}

void LogWidgetBasic::createTime(int row, QString time, TimeMode timeMode)
{
    if( timeMode == SET ) {
        createItem(time, row, TIME, true);
    } else if( timeMode == SETNOTEDITABLE ){
        createItem(time, row, TIME, false);
    }
}

void LogWidgetBasic::createEntry(int row, Mode mode, Type type) // FIXME mode is not used any more
{
    QString msg;

    if(type == RECORDSTART)
        msg = QDate::currentDate().toString()+" - ";
    msg.append(ui->entryLine->text());
    createItem(msg, row, ENTRY, true);
}

void LogWidgetBasic::createEntry(int row, QString txt)
{
    createItem(txt, row, ENTRY, true);
}

void LogWidgetBasic::createDel(int row, Type type)
{
    if(type != RECORDSTART && type != RECORDEND){
        createItem("", row, DEL, false, QIcon(":/icons/Delete.png"));
    } else {
        createItem("", row, DEL, false);
    }
}

void LogWidgetBasic::on_entryLine_returnPressed()
{
    if(ui->motionStartButton->isEnabled()){
        on_motionStartButton_clicked();
    } else if(ui->motionEndButton->isEnabled()){
        on_motionEndButton_clicked();
    } else if(!ui->entryLine->text().isNull()){
        createItems(-1, NORMAL, TEXT, EMPTY);
    }

    entryLineInit();
}

void LogWidgetBasic::checkConsistency() const {

    QString errorMsg;

    if (recordID != INVALID_RECORD_ID) {

        errorMsg += "record ID not cleared; ";
    }

    if (!errorMsg.isEmpty()) {

        //fatalError(errorMsg);
    }
}

void LogWidgetBasic::on_recStartButton_clicked()
{
    createItems(-1, NORMAL, RECORDSTART, EMPTY);

    ui->recStartButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->motionStartButton->setFocus();
    ui->saveButton->setEnabled(false);
    ui->clearButton->setEnabled(false);

    entryLineInit();

    connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)),
            &application.dataRecorder,   SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);

}

void LogWidgetBasic::on_recEndButton_clicked()
{

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
    ui->clearButton->setEnabled(true);

    for(int i=0; i<ui->log->rowCount(); i++){
        ui->log->item(i, ENTRY)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    for(int i=1; i<ui->log->rowCount()-1; i++){
        ui->log->item(i, TIME)->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    //disconnect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

    entryLineInit();
}

void LogWidgetBasic::on_motionStartButton_clicked()
{
    qDebug() << "Mot Start";
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItems(-1, NORMAL, MOTIONSTART, UNKNOWN);
    ui->motionStartButton->setEnabled(false);
    ui->motionEndButton->setEnabled(true);
    ui->motionEndButton->setFocus();
    ui->recEndButton->setEnabled(false);

    entryLineInit();
}

void LogWidgetBasic::on_motionEndButton_clicked()
{
    qDebug() << "Mot end";
    QString msg;
    if(!(ui->entryLine->text() == "")) msg.append(" - "+ui->entryLine->text());
    createItems(-1, NORMAL, MOTIONEND, EMPTY);
    ui->motionEndButton->setEnabled(false);
    ui->motionStartButton->setEnabled(true);
    ui->motionStartButton->setFocus();
    ui->recEndButton->setEnabled(true);

    entryLineInit();

    ui->log->update();
}

void LogWidgetBasic::on_saveButton_clicked()
{
    qDebug() << "Save";
    //connect(ui->log, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
    //QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/"+ui->log->item(0,ENTRY)->text()+".csv", "CSV (*.csv)");

    getUniqueRecordID();

    QString fn = "../rec/"+QString::number(recordID)+".csv";

    saveLog( fn );

    ui->recStartButton->setEnabled(false);
    //ui->entryLine->setEnabled(true);
    //connect(ui->entryLine, SIGNAL(returnPressed()), this, SLOT(on_entryLine_returnPressed()));

    entryLineInit();
}

void LogWidgetBasic::getUniqueRecordID() {

    const QString angles = anglesCSV();

    if (recordID > 0) {

        Q_ASSERT(QFile::exists("../rec/"+QString::number(recordID)+".csv"));

        recSelect->updateRecord(recordID, angles);
    }
    else {

        //recordID = recSelect->insertRecord(person.id(), getMotionType(), angles);
    }
}

void LogWidgetBasic::on_clearButton_clicked()
{
    qDebug() << "Clear All";



        clearLog();

}


bool LogWidgetBasic::clearLog() {

    qDebug() << "clearLog()";

    init();

    recordID = INVALID_RECORD_ID;

    return true;
}

void LogWidgetBasic::onDelRow(int row)
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

            markUnsaved();
        }
    }

    entryLineInit();
}

void LogWidgetBasic::on_log_cellClicked(int row, int column)
{
    if(column == DEL){
        onDelRow(row);
    }
}

void LogWidgetBasic::on_log_cellChanged(int row, int column)
{
    QTableWidgetItem* item = ui->log->item(row, column);
    if(column == TIME && item->isSelected() && row < ui->log->rowCount()){
        QTime before = QTime::fromString(ui->log->item(row-1,column)->text(), "hh:mm:ss"); // FIXME Crashes on this line
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
        ui->log->item(findMotionStart(row), STATUS)->setText(UNKNOWN_TEXT);
        ui->log->item(findMotionStart(row), STATUS)->setIcon(QIcon(":/icons/Warning.png"));
    }

    markUnsaved();
}

void LogWidgetBasic::displayWarning(const QString& msg) {

    QMessageBox::warning(this, "Error", msg);
}

int LogWidgetBasic::findMotionStart(int endRow)
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

int LogWidgetBasic::findMotionEnd(int startRow)
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

void LogWidgetBasic::ShowContextMenu(const QPoint& pos)
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

bool LogWidgetBasic::isRecordEnd(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Record End", Qt::CaseSensitive);
}

bool LogWidgetBasic::isRecordStart(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Record Start", Qt::CaseSensitive);
}

bool LogWidgetBasic::isMotionStart(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Motion Start", Qt::CaseSensitive);
}

bool LogWidgetBasic::isMotionEnd(int row) const
{
    return ui->log->item(row,TYPE)->text().contains("Motion End", Qt::CaseSensitive);
}

void LogWidgetBasic::saveLog(const QString &filename)
{
    application.dataRecorder.saveSamples(filename);

    QFile f( filename );

    if( !f.open( QIODevice::Append ) ) {

        return;
    }

    QTextStream ts( &f );

    ts << "#LOG metadata" << endl;

    ts << "#Status,Time,Type,Entry" << endl;
    for (int i=0; i<ui->log->rowCount(); i++){ // FIXME Crashes in the loop: item is null pointer
        ts << ui->log->item(i,STATUS)->text() << "," << ui->log->item(i,TIME)->text() << "," << ui->log->item(i,TYPE)->text() << "," << ui->log->item(i,ENTRY)->text() << endl;
    }

    ts << "#Record ID" << endl;
    ts << recordID << endl;

    ts << "#Time,FLEX_MIN,FLEX_MAX, ..., MED_MAX" << endl;
    int motEnd = 0;
    int motStart = findMotStart(motEnd);
    while(motStart != NO_MORE) {
        ts << timeInRow(motStart).toString() << ",";

        if ( computeExtrema(motStart) ){
            for(int i = 0; i < SIZE_OF_ARRAY; i++){
                ts << extrema[i] << ",";
            }
        }
        ts << endl;
        motEnd = findMotionEnd(motStart);
        motStart = findMotStart(motEnd);
    }

    ts.flush();
    f.close();

    markSaved();
}

//=============================================================================

void LogWidgetBasic::startChecking() {

    startAt = endAt = -1;

    writeToConsole(" checking started");

    checkNextMotion();
}

int LogWidgetBasic::findNextMot() {

    startAt = findMotStart(endAt+1);

    if (startAt == NO_MORE) {

        return NO_MORE;
    }

    endAt = findMotEnd(startAt+1);

    return startAt;
}

void LogWidgetBasic::checkNextMotion() {

    if (findNextMot() == NO_MORE) {

        //finishedChecking();
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

void LogWidgetBasic::startSolver() {

    int begin = recStart().secsTo(motionStart());

    int end = recStart().secsTo(motionEnd());

    bool error = application.solver.start(begin, end, recLengthInSec());

    if (error) {

        //finishedChecking();
    }
    else {

        blockingBox->show();
    }
}


void LogWidgetBasic::markAsFailed() {

    setText(startAt, STATUS, FAILED_TEXT);

    setIcon(startAt, STATUS, QIcon(":/icons/failed.png"));

    markUnsaved();
}

void LogWidgetBasic::motionOK() {

    setText(startAt, STATUS, PASSED_TEXT);

    setIcon(startAt, STATUS, QIcon(":/icons/passed.png"));

    markUnsaved();
}

void LogWidgetBasic::setText(int row, Column col, const char text[]) {

    item(row, col).setText(text);
}

void LogWidgetBasic::setIcon(int row, Column col, const QIcon& icon) {

    item(row, col).setIcon(icon);
}

const QTime LogWidgetBasic::motionStart() const {

    Q_ASSERT(isMotionStart(startAt));

    return timeInRow(startAt);
}

const QTime LogWidgetBasic::motionEnd() const {

    Q_ASSERT(isMotionEnd(endAt));

    return timeInRow(endAt);
}

bool LogWidgetBasic::isMotionTooShort() const {

    return motionStart().secsTo(motionEnd()) <= TOO_SHORT_IN_SEC;
}

int LogWidgetBasic::findMotStart(int pos) const {

    for ( ; pos < rowCount(); ++pos ) {

        if( isMotionStart(pos) ) {

            return pos;
        }
    }

    return NO_MORE;
}

int LogWidgetBasic::findMotEnd(int pos) const {

    for ( ; pos < rowCount(); ++pos ) {

        if( isMotionEnd(pos) ) {

            return pos;
        }
    }

    Q_ASSERT(false);

    return NO_MORE;
}

const QTableWidget& LogWidgetBasic::tableWidget() const {

    return *(ui->log);
}

const QTableWidgetItem& LogWidgetBasic::item(int row, Column col) const {

    return *(tableWidget().item(row, col));
}

QTableWidgetItem& LogWidgetBasic::item(int row, Column col) {

    return *(ui->log->item(row, col));
}

const QTime LogWidgetBasic::timeInRow(int row) const {

    return QTime::fromString(item(row,TIME).text(), "hh:mm:ss"); // FIXME Why isn't it stored as data???
}

int LogWidgetBasic::rowCount() const {

    return tableWidget().rowCount();
}

bool LogWidgetBasic::isAlreadyPassed(const int row) const {

    return item(row, STATUS).text().contains(PASSED_TEXT, Qt::CaseSensitive);
}

int LogWidgetBasic::recLengthInSec() const {

    const int lastRow = rowCount() - 1;

    Q_ASSERT(lastRow > 0);

    Q_ASSERT(isRecordEnd(lastRow));

    const QTime recEnd = timeInRow(lastRow);

    int recLength = recStart().secsTo(recEnd);

    Q_ASSERT(recLength >= 0);

    return recLength;
}

const QTime LogWidgetBasic::recStart() const {

    Q_ASSERT(rowCount() > 2);

    Q_ASSERT(isRecordStart(0));

    return timeInRow(0);
}

const QString LogWidgetBasic::atRow() const {

    return QString(" - row " + QString::number(startAt) + " ");
}

void LogWidgetBasic::writeToConsole(const QString& msg) const {

    QString time = QDateTime::currentDateTime().time().toString();

    application.showConsoleMessage( time + msg );
}

void LogWidgetBasic::checkRange(const int begin, const int end) const {

    const int length = recLengthInSec();

    if (!(0 <= begin && begin < end && end <= length)) {

        QString msg;

        QTextStream out(&msg, QIODevice::WriteOnly);

        out << " begin " << begin << ", end " << end << "length " << length << " s" << flush;

        //fatalError(msg);
    }
}

const Range LogWidgetBasic::motionBeginEndInSamples(const int motStart) const {

    Q_ASSERT(isMotionStart(motStart));

    int motEnd = findMotEnd(motStart+1);

    int begin = recStart().secsTo(timeInRow(motStart));

    int end   = recStart().secsTo(timeInRow(motEnd));

    checkRange(begin, end);

    const Range rangeInSec(begin, end);

    return application.dataRecorder.rangeInSample(rangeInSec, recLengthInSec());
}

bool LogWidgetBasic::computeExtrema(int motStart) {

    if (!isAlreadyPassed(motStart)) {

        return false;
    }

    DataHolder* data = getDataHolder(motStart);

    const double* const minMax = data->min_max();

    for (int i=0; i<SIZE_OF_ARRAY; ++i) {

        extrema[i] = minMax[i];
    }

    delete data;

    return true;
}

const QString LogWidgetBasic::anglesCSV() {

    QString result;

    const int motStart = findMotStart(0);

    if (motStart==NO_MORE || !isAlreadyPassed(motStart)) {

        return result;
    }

    DataHolder* data = getDataHolder(motStart);

    result = data->angles_in_csv().c_str();

    delete data;

    return result;
}

void LogWidgetBasic::on_log_cellDoubleClicked(int motStart, int column) {

    if (column != STATUS || !isAlreadyPassed(motStart)) {

        return;
    }

    item(motStart, STATUS).setSelected(true);

    showAnimation(motStart);
}

void LogWidgetBasic::showAnimation(const int motStartRow) {

    if (application.dataRecorder.empty()) {

        writeToConsole("Error: datarecorder is empty!");

        return;
    }

    const Range range = motionBeginEndInSamples(motStartRow);

    double* const mat = application.dataRecorder.rotmat(range);

    GLWindow* win = getGLWindow(mat, range.size());

    //win->setWindowTitle(person.name()+"  "+person.birth().toString(Qt::ISODate));

    win->showMaximized();

    win->activateWindow();
}

GLWindow* LogWidgetBasic::getGLWindow(double* mat, int size) const {

    GLWindow* win = 0;

//    const MotionType type = getMotionType();
//
//    if (type == RIGHT_ELBOW_FLEX) {
//
//        win = GLWindow::right(mat, size);
//    }
//    else if (type == LEFT_ELBOW_FLEX) {
//
//        win = GLWindow::left(mat, size);
//    }
//    else {
//
//        fatalError("unknown motion type");
//    }

    return win;
}

DataHolder* LogWidgetBasic::getDataHolder(const int motStart) const {

    const Range range = motionBeginEndInSamples(motStart);

    double* const mat = application.dataRecorder.rotmat(range);

    const int size = range.size();

    DataHolder* data = 0;

    //const MotionType type = getMotionType();

//    if (type == RIGHT_ELBOW_FLEX) {
//
//        data = DataHolder::right(mat, size);
//    }
//    else if (type == LEFT_ELBOW_FLEX) {
//
//        data = DataHolder::left(mat, size);
//    }
//    else {
//
//        fatalError("unknown motion type");
//    }

    return data;
}

