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

#include <QDebug>
#include <QFile>
#include <QFontMetrics>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include "RecordHandler.hpp"
#include "CustomSqlQueryModel.hpp"
#include "Person.hpp"

namespace {

const char SELECT[] = "SELECT record.id, person.id, person.name, person.birthday, motion.type, record.date_added "

                      "FROM record JOIN person ON (record.person=person.id) JOIN motion ON (record.type=motion.id) ";

const char ORDER_BY[] = "ORDER BY person.name, person.birthday, record.date_added DESC";

enum Columns {
    REC_ID,
    PERSON_ID,
    NAME,
    BIRTH,
    TYPE,
    ADDED,
    NUMBER_OF_COLUMNS
};

}

RecordHandler::RecordHandler() :
        model(0),
        view(new QTableView),
        nameInput(new QLineEdit),
        clearBtn(createButton("Clear")),
        delBtn(createButton("Delete selected")),
        sqlQuery(new QSqlQuery)
{
    QVBoxLayout* layout = new QVBoxLayout;

    layout->addLayout( createInputLine() );

    layout->addLayout( createControlButtons() );

    layout->addWidget(new QLabel("Double-click on a record to use:"));

    setupModel();

    setupView();

    layout->addWidget(view);

    setLayout(layout);

    setWindowModality(Qt::ApplicationModal);
}

RecordHandler::~RecordHandler() {

    // FIXME Resources are leaked -- not clear how to reclaim them
}

void RecordHandler::checkForError(const QSqlError& error) {

    if (error.isValid()) {

        displayError(error.databaseText()+'\n'+error.driverText());
    }
}

QHBoxLayout* RecordHandler::createInputLine() {

    QHBoxLayout* line = new QHBoxLayout;

    line->addWidget(new QLabel("Name: "));

    connect(nameInput, SIGNAL(textChanged(QString)), SLOT(nameEdited(QString)));

    connect(nameInput, SIGNAL(returnPressed()), SLOT(probablyDone()));

    line->addWidget(nameInput);

    return line;
}

QHBoxLayout* RecordHandler::createControlButtons() {

    QHBoxLayout* buttons = new QHBoxLayout;

    connect(clearBtn, SIGNAL(clicked()), SLOT(clearClicked()));

    connect(delBtn, SIGNAL(clicked()), SLOT(deleteClicked()));

    buttons->addWidget(clearBtn);

    buttons->addWidget(delBtn);

    buttons->addStretch();

    return buttons;
}

QPushButton* RecordHandler::createButton(const char text[]) const {

    QPushButton* button = new QPushButton(text);

    button->setFixedWidth(pixelWidth(text) + 20);

    return button;
}

void RecordHandler::setupModel() {

    CustomSqlQueryModel<Columns, NUMBER_OF_COLUMNS>* customModel = new CustomSqlQueryModel<Columns, NUMBER_OF_COLUMNS>;

    customModel->setAlignment(BIRTH, Qt::AlignRight | Qt::AlignVCenter);

    customModel->setAlignment(TYPE, Qt::AlignRight | Qt::AlignVCenter);

    customModel->setAlignment(ADDED, Qt::AlignHCenter | Qt::AlignVCenter);

    model = customModel;

    setSelectQuery("");

    model->setHeaderData(REC_ID, Qt::Horizontal, "Rec ID", Qt::DisplayRole);

    model->setHeaderData(PERSON_ID, Qt::Horizontal, "Person ID");

    model->setHeaderData(NAME, Qt::Horizontal, "Name");

    model->setHeaderData(BIRTH, Qt::Horizontal, "Date of birth");

    model->setHeaderData(TYPE, Qt::Horizontal, "Type of motion");

    model->setHeaderData(ADDED, Qt::Horizontal, "Recorded on");

}

void RecordHandler::setupView() {

    view->setModel(model);

    view->verticalHeader()->hide();

    view->hideColumn(REC_ID);

    view->hideColumn(PERSON_ID);

    view->horizontalHeader()->setStretchLastSection(true);

    view->resizeColumnsToContents();

    int birthWidth = pixelWidth("     2000-00-00");

    view->setColumnWidth(BIRTH, birthWidth);

    int typeWidth = pixelWidth("     RIGHT_ELBOW_FLEX");

    view->setColumnWidth(TYPE, typeWidth);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->setSelectionMode(QAbstractItemView::SingleSelection);

    view->setAlternatingRowColors(true);

    connect(view, SIGNAL(activated(QModelIndex)), SLOT(itemActivated(QModelIndex)) );
}

void RecordHandler::setSelectQuery(const QString& whereClause) {

    const QString query = QString(SELECT)+whereClause+QString(ORDER_BY);

    qDebug() << "executing: " << query;

    model->setQuery(query);

    checkForError(model->lastError());
}

void RecordHandler::setSelectQueryLikeName() {

    const QString name = this->name();

    if (name.length()==0) {

        setSelectQuery("");
    }
    else {

        setSelectQuery("WHERE name LIKE '"+name+"%' ");
    }
}

void RecordHandler::executeRawSQL(const QString& rawSQL) {

    qDebug() << "Raw SQL: " << rawSQL;

    sqlQuery->exec(rawSQL);

    checkForError(sqlQuery->lastError());
}

qint64 RecordHandler::lastInsertID() const {

    QVariant newID = sqlQuery->lastInsertId();

    return newID.isValid() ? toInt64(newID) : -1;
}

int RecordHandler::rowsAffected() const {

    return sqlQuery->numRowsAffected();
}

const QString RecordHandler::anglesValue(const QString& angles) const {

    return (angles.isEmpty()) ? "NULL" : "'"+angles+"'" ;
}

qint64 RecordHandler::insertRecord(qint64 personID, MotionType type, const QString& angles) {

    const QString angl = anglesValue(angles);

    QString stmt = "INSERT INTO record VALUES (NULL, "+
                                               QString::number(personID)+", "+
                                               QString::number(type)+
                                               ", DATETIME('now', 'localtime'), "+
                                               angl+");";

    executeRawSQL(stmt);

    qint64 recID = lastInsertID();

    Q_ASSERT(recID > 0);

    setSelectQueryLikeName();

    return recID;
}

void RecordHandler::updateRecord(qint64 recID, const QString& angles) {

    const QString angl = anglesValue(angles);

    QString stmt = "UPDATE record SET angles="+angl+" WHERE id="+QString::number(recID)+";";

    executeRawSQL(stmt);

    const int affected_rows = rowsAffected();

    Q_ASSERT(affected_rows==1);

    setSelectQueryLikeName();
}

void RecordHandler::nameEdited(const QString& ) {

    setSelectQueryLikeName();
}

void RecordHandler::itemActivated(const QModelIndex& item) {

    const int row = item.row();

    emit recordSelected(getRecordID(row), getPerson(row), getMotionType(row));

    close();
}

void RecordHandler::probablyDone() {

    if (model->rowCount()==1) {

        emit recordSelected(getRecordID(0), getPerson(0), getMotionType(0));

        close();
    }
}

void RecordHandler::clearClicked() {

    clearInput();

    setSelectQueryLikeName();
}

void RecordHandler::clearInput() {

    nameInput->clear();

    nameInput->setFocus();

    view->clearSelection();
}

void RecordHandler::deleteClicked() {

    QModelIndexList selected = view->selectionModel()->selectedIndexes();

    if (selected.empty()) {

        displayWarning("Please select a row to delete!");

        return;
    }

    if (displayQuestion("Are you sure you want to delete the selected record?"))
    {
        qint64 id = getRecordID(selected.first().row());

        deleteRecord(id);
    }
}

void RecordHandler::deleteRecord(const qint64 id) {

    Q_ASSERT(id > 0);

    qDebug() << "Deleting record " << id << ".csv";

    const QString filename = "../rec/"+QString::number(id)+".csv";

    if (!QFile::remove(filename)) {

        qDebug() << "Failed to delete!";
    }

    executeRawSQL("DELETE FROM record WHERE id = "+QString::number(id));

    setSelectQueryLikeName();
}

QSize RecordHandler::minimumSizeHint() const {

    return QSize(300, 300);
}

QSize RecordHandler::sizeHint() const {

    return QSize(750, 700);
}

const QDate RecordHandler::getDate(int row) const {

    Q_ASSERT( 0<=row && row < model->rowCount() );

    QModelIndex birthCol = model->index(row, BIRTH);

    return model->data(birthCol).toDate();
}

const QString RecordHandler::getName(int row) const {

    Q_ASSERT( 0<=row && row < model->rowCount() );

    QModelIndex nameCol = model->index(row, NAME);

    return model->data(nameCol).toString();
}

// FIXME Hideous workaround
MotionType RecordHandler::getMotionType(const int row) const {

    QModelIndex item = model->index(row, TYPE);

    const QString type = model->data(item).toString();

    MotionType result;

    if (type=="RIGHT_ELBOW_FLEX") {

        result = RIGHT_ELBOW_FLEX;
    }
    else if (type=="LEFT_ELBOW_FLEX") {

        result = LEFT_ELBOW_FLEX;
    }
    else {

        displayError("Unknown motion type "+type);
    }

    return result;
}

const Person RecordHandler::getPerson(const int row) const {

    qint64 id = getPersonID(row);

    QString name = getName(row);

    QDate birth = getDate(row);

    return Person(id, name, birth);
}

qint64 RecordHandler::getID(int row, int col) const {

    Q_ASSERT( 0<=row && row < model->rowCount() );
    Q_ASSERT( 0<=col && col < model->columnCount());

    QModelIndex idCell = model->index(row, col);

    QVariant id = model->data(idCell);

    return toInt64(id);
}

qint64 RecordHandler::getRecordID(int row) const {

    return getID(row, static_cast<int>(REC_ID));
}

qint64 RecordHandler::getPersonID(int row) const {

    return getID(row, static_cast<int>(PERSON_ID));
}

qint64 RecordHandler::toInt64(const QVariant& var) const {

    bool success = false;

    qint64 int64Value = var.toLongLong(&success);

    if (!success) {

        displayError("Failed to convert the ID to int64");
    }

    Q_ASSERT(int64Value > 0);

    return int64Value;
}

int RecordHandler::pixelWidth(const char text[]) const {

    QFont defaultFont;

    QFontMetrics fm(defaultFont);

    return fm.width(text);
}

const QString RecordHandler::name() const {

    return nameInput->text().toUpper().trimmed();
}

void RecordHandler::displayError(const QString& msg) const {

    QMessageBox::critical(0, "Fatal error", msg);
    
    exit(EXIT_FAILURE);
}

void RecordHandler::displayWarning(const QString& msg) {

    QMessageBox::warning(this, "Error", msg);
}

bool RecordHandler::displayQuestion(const QString& question) {

    int ret = QMessageBox::question(this, "Warning", question, QMessageBox::Yes, QMessageBox::Cancel);

    return (ret == QMessageBox::Yes)? true : false ;
}

void RecordHandler::showEvent(QShowEvent* event) {

    setSelectQueryLikeName();

    QWidget::showEvent(event);
}

void RecordHandler::closeEvent(QCloseEvent* event) {

    clearInput();

    QWidget::closeEvent(event);
}
