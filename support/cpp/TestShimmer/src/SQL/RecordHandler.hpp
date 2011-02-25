/* Copyright (c) 2011 University of Szeged
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
* Author: Ali Baharev
*/

#ifndef RECORDSELECTOR_HPP
#define RECORDSELECTOR_HPP

#include <QString>
#include <QDate>
#include "MotionTypes.hpp"

class Person;
class QHBoxLayout;
class QLineEdit;
class QModelIndex;
class QSqlQueryModel;
class QSqlError;
class QTableView;
class QPushButton;

#include <QWidget>

class RecordHandler : public QWidget {

    Q_OBJECT

public:

    RecordHandler();

    qint64 insertRecord(qint64 personID, MotionType type);

    ~RecordHandler();

    QSize minimumSizeHint() const;

    QSize sizeHint() const;

signals:

    void recordSelected(qint64 recordID, const Person& person);

private slots:

    void nameEdited(const QString& name);

    void probablyDone();

    void itemActivated(const QModelIndex & index);

    void clearClicked();

    void deleteClicked();

    void showEvent(QShowEvent* event);

    void closeEvent(QCloseEvent* event);

private:

    Q_DISABLE_COPY(RecordHandler);

    void setupModel();
    QHBoxLayout* createInputLine();
    void setupView();
    QHBoxLayout* createControlButtons();
    QPushButton* createButton(const char text[]) const;

    void clearInput();

    void setSelectQuery(const QString& whereClause);
    void setSelectQueryLikeName();
    void checkForError(const QSqlError& error);
    qint64 executeRawSQL(const QString& rawSQL);

    void deleteRecordCSV(const qint64 id);
    void deleteRecord(const qint64 id);

    const QString getName(int row) const;
    const QDate getDate(int row) const;
    const Person getPerson(const int row) const;
    qint64 getID(int row, int col) const;
    qint64 getPersonID(int row) const;
    qint64 getRecordID(int row) const;
    int pixelWidth(const char text[]) const;
    const QString name() const;
    qint64 toInt64(const QVariant& var) const;

    void displayError(const QString& msg) const;
    void displayWarning(const QString& msg);
    bool displayQuestion(const QString& question);

    QSqlQueryModel* model;
    QTableView* const view;
    QLineEdit* const nameInput;
    QPushButton* const clearBtn;
    QPushButton* const delBtn;

};

#endif // RECORDSELECTOR_HPP
