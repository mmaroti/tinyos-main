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

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include "StateColor.hpp"

class QMessageBox;
class Application;

namespace Ui{
    class LogWidget;
}

class LogWidget : public QWidget {

    Q_OBJECT

public:

    LogWidget(QWidget *parent, Application &app);
    ~LogWidget();

    enum Button { MotionStart, MotionEnd, RecordStart, RecordEnd, Text, Insert, Load };
    enum Column {
        GOTO = 0,
        STATUS = 1,
        TIME = 2,
        TYPE = 3,
        ENTRY = 4,
        DEL = 5 };
    enum Status { UNKNOWN, OK, FAILED };

private slots:

    void on_entryLine_returnPressed();
    void on_recStartButton_clicked();
    void on_recEndButton_clicked();
    void on_motionStartButton_clicked();
    void on_motionEndButton_clicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    void on_clearButton_clicked();
    void on_checkButton_clicked();

    void ShowContextMenu(const QPoint& pos);
    void on_log_cellClicked(int,int);
    void on_log_cellChanged(int,int);
    void stateColor(StateColor color);
    void solverFinished(bool error, const QString message);

private:

    void onGoto(int);
    void onDelRow(int);

    void createItem(QString status, QString text, QString time, Button button, int at);

    int findRecordStart();
    int findRecordEnd();
    int findMotionStart(int);
    int findMotionEnd(int);
    int motionDistance(int, int);

    bool isRecordStart(int);
    bool isRecordEnd(int);
    bool isMotionStart(int);
    bool isMotionEnd(int);

    void init();
    void saveLog(const QString&);
    void loadLog(const QString&);
    void csvToLog(const QString&);

    //=======================
    void startChecking();
    void checkNextMotion();
    void finishedChecking();

    void startSolver();
    void markAsFailed();
    void motionOK();

    int findNextMot();
    int findMotStart(int from);
    int findMotEnd(int from);

    const QTime motionStart();
    const QTime motionEnd();
    const QTime recStart();
    const QTime timeInRow(int row);
    bool isMotionTooShort();
    bool isAlreadyPassed();

    int rowCount();
    int recLengthInSec();

    QTableWidget& tableWidget();
    QTableWidgetItem& item(int row, Column col);
    void writeToConsole(const QString& msg);
    const QString atRow() const;
    //=======================

    Ui::LogWidget *ui;
    Application &application;
    bool inEditing;

    //=======================
    QMessageBox* blockingBox;
    int startAt;
    int endAt;
};

#endif // LOGWIDGET_H
