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

#include <QDateEdit>
#include <QDebug>
#include <QDir>
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
#include "SQLDialog.hpp"
#include "CustomSqlQueryModel.hpp"
#include "Person.hpp"

namespace {

const char DATABASE[] = "QSQLITE";

const char DB_NAME[] = "../rec/records.sqlite"; // FIXME Hard-coded constant

const char SELECT[] = "SELECT id, name, birthday, date_added FROM person ";

const char ORDER_BY[] = "ORDER BY name, birthday";

enum Columns {
    ID,
    NAME,
    BIRTH,
    ADDED,
    NUMBER_OF_COLUMNS
};

}

SQLDialog::SQLDialog() :
        today(QDate::currentDate()),
        model(0),
        view(new QTableView),
        nameInput(new QLineEdit),
        dateInput(new QDateEdit(today)),
        clearBtn(createButton("Clear")),
        newBtn(createButton("New person")),
        delBtn(createButton("Delete selected"))
{
    connectToDatabase();

    QVBoxLayout* layout = new QVBoxLayout;

    layout->addLayout( createInputLine() );

    layout->addLayout( createControlButtons() );

    layout->addWidget(new QLabel("Double-click on a person to use:"));

    setupModel();

    setupView();

    layout->addWidget(view);

    setLayout(layout);

    setWindowModality(Qt::ApplicationModal);
}

SQLDialog::~SQLDialog() {

    // FIXME Resources are leaked -- not clear how to reclaim them
}

void SQLDialog::checkDBexistence() {

    if (QFile::exists(DB_NAME)) {

        return;
    }

    QString msg("Failed to open ");

    msg.append(DB_NAME);

    msg.append("\nCurrent working directory is:\n");

    msg.append(QDir::currentPath());

    displayError(msg);
}

void SQLDialog::connectToDatabase() {

    checkDBexistence();

    QSqlDatabase db = QSqlDatabase::addDatabase(DATABASE);

    db.setDatabaseName(DB_NAME);

    if (!db.open()) {

        displayError("Failed to open the database!");
    }

    QSqlQuery sql("PRAGMA foreign_keys = ON");

    checkForError(sql.lastError());
}

void SQLDialog::checkForError(const QSqlError& error) {

    if (error.isValid()) {

        displayError(error.databaseText()+'\n'+error.driverText());
    }
}

QHBoxLayout* SQLDialog::createInputLine() {

    QHBoxLayout* line = new QHBoxLayout;

    line->addWidget(new QLabel("Name: "));

    connect(nameInput, SIGNAL(textChanged(QString)), SLOT(nameEdited(QString)));

    line->addWidget(nameInput);

    line->addWidget(new QLabel("Born (YYYY-MM-DD): "));

    dateInput->setDisplayFormat("yyyy-MM-dd");

    dateInput->setMaximumDate(today);

    line->addWidget(dateInput);

    return line;
}

QHBoxLayout* SQLDialog::createControlButtons() {

    QHBoxLayout* buttons = new QHBoxLayout;

    connect(clearBtn, SIGNAL(clicked()), SLOT(clearClicked()));

    connect(newBtn, SIGNAL(clicked()), SLOT(newPerson()));

    connect(delBtn, SIGNAL(clicked()), SLOT(deleteClicked()));

    buttons->addWidget(newBtn);

    buttons->addWidget(clearBtn);

    buttons->addWidget(delBtn);

    buttons->addStretch();

    return buttons;
}

QPushButton* SQLDialog::createButton(const char text[]) const {

    QPushButton* button = new QPushButton(text);

    button->setFixedWidth(pixelWidth(text) + 20);

    return button;
}

void SQLDialog::setupModel() {

    CustomSqlQueryModel<Columns, NUMBER_OF_COLUMNS>* customModel = new CustomSqlQueryModel<Columns, NUMBER_OF_COLUMNS>;

    customModel->setAlignment(BIRTH, Qt::AlignRight | Qt::AlignVCenter);

    customModel->setAlignment(ADDED, Qt::AlignHCenter | Qt::AlignVCenter);

    model = customModel;

    setSelectQuery("");

    model->setHeaderData(NAME, Qt::Horizontal, "Name");

    model->setHeaderData(BIRTH, Qt::Horizontal, "Date of birth");

    model->setHeaderData(ADDED, Qt::Horizontal, "Added on");

}

void SQLDialog::setupView() {

    view->setModel(model);

    view->verticalHeader()->hide();

    view->hideColumn(ID);

    view->horizontalHeader()->setStretchLastSection(true);

    view->resizeColumnsToContents();

    int pixelsWide = pixelWidth("     2000-00-00");

    view->setColumnWidth(BIRTH, pixelsWide);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);

    view->setSelectionMode(QAbstractItemView::SingleSelection);

    view->setAlternatingRowColors(true);

    connect(view, SIGNAL(activated(QModelIndex)), SLOT(itemActivated(QModelIndex)) );
}

void SQLDialog::setSelectQuery(const QString& whereClause) {

    const QString query = QString(SELECT)+whereClause+QString(ORDER_BY);

    qDebug() << "executing: " << query;

    model->setQuery(query);

    checkForError(model->lastError());
}

void SQLDialog::setSelectQueryLikeName() {

    const QString name = this->name();

    if (name.length()==0) {

        setSelectQuery("");
    }
    else {

        setSelectQuery("WHERE name LIKE '"+name+"%' ");
    }
}

qint64 SQLDialog::executeRawSQL(const QString& rawSQL) {

    qDebug() << "Raw SQL: " << rawSQL;

    QSqlQuery sql(rawSQL);

    checkForError(sql.lastError());

    QVariant newID = sql.lastInsertId();

    qint64 id = newID.isValid() ? toInt64(newID) : -1;

    setSelectQueryLikeName();

    return id;
}

void SQLDialog::nameEdited(const QString& ) {

    setSelectQueryLikeName();

    dateInput->setDate(today);
}

void SQLDialog::itemActivated(const QModelIndex& item) {

    const int row = item.row();

    emit personSelected(getPerson(row));

    close();
}

void SQLDialog::clearClicked() {

    clearInput();

    setSelectQueryLikeName();
}

void SQLDialog::clearInput() {

    nameInput->clear();

    nameInput->setFocus();

    dateInput->setDate(today);

    view->clearSelection();
}

void SQLDialog::newPerson() {

    const QString name = this->name();

    if (name.length()==0) {
        displayWarning("Please enter a name!");
        nameInput->setFocus();
        return;
    }

    const QDate birth = dateInput->date();

    if (birth==today) {
        displayWarning("Please check the date of birth!");
        dateInput->setFocus();
        return;
    }

    const QString dateOfBirth = birth.toString(Qt::ISODate);

    setSelectQuery("WHERE name = '"+name+"' AND birthday=DATE('"+dateOfBirth+"') ");

    if (model->rowCount()!=0) {
        displayWarning("Please make sure this person is not already in the database!");
        nameInput->setFocus();
        return;
    }

    insertNewPerson(name, dateOfBirth);
}

void SQLDialog::insertNewPerson(const QString& name, const QString& birth) {

    qint64 id = executeRawSQL("INSERT INTO person VALUES (NULL, '"+name+"', DATE('"+birth+"'), DATETIME('now', 'localtime') );");

    Q_ASSERT (id > 0);

    close();

    emit personSelected(Person(id, name, dateInput->date()));
}

void SQLDialog::deleteClicked() {

    QModelIndexList selected = view->selectionModel()->selectedIndexes();

    if (selected.empty()) {

        displayWarning("Please select a row to delete!");

        return;
    }

    if (displayQuestion("Are you sure you want to delete the selected row and "
                        "ALL THE RECORDS belonging to it?"))
    {
        qint64 id = getPersonID(selected.first().row());

        deleteRecordsOfPerson(id);

        deletePerson(id);
    }
}

void SQLDialog::deleteRecordsOfPerson(const qint64 id) {

    QSqlQuery query("SELECT id FROM record WHERE person="+QString::number(id)+";");

    checkForError(query.lastError());

    while (query.next()) {

        qint64 id = toInt64(query.value(0));

        deleteRecordCSV(id);
    }
}

void SQLDialog::deleteRecordCSV(const qint64 id) {

    qDebug() << "Deleting record " << id << ".csv";

    const QString filename = "../rec/"+QString::number(id)+".csv";

    if (!QFile::remove(filename)) {

        qDebug() << "Failed to delete!";
    }
}

void SQLDialog::deletePerson(const qint64 id) {

    executeRawSQL("DELETE FROM person WHERE id = "+QString::number(id));
}

QSize SQLDialog::minimumSizeHint() const {

    return QSize(300, 300);
}

QSize SQLDialog::sizeHint() const {

    return QSize(750, 700);
}

const QDate SQLDialog::getDate(int row) const {

    Q_ASSERT( 0<=row && row < model->rowCount() );

    QModelIndex birthCol = model->index(row, BIRTH);

    return model->data(birthCol).toDate();
}

const QString SQLDialog::getName(int row) const {

    Q_ASSERT( 0<=row && row < model->rowCount() );

    QModelIndex nameCol = model->index(row, NAME);

    return model->data(nameCol).toString();
}

const Person SQLDialog::getPerson(const int row) {

    qint64 id = getPersonID(row);

    QString name = getName(row);

    QDate birth = getDate(row);

    return Person(id, name, birth);
}

qint64 SQLDialog::getPersonID(int row) {

    Q_ASSERT( 0<=row && row < model->rowCount() );

    QModelIndex idCol = model->index(row, ID);

    QVariant personID = model->data(idCol);

    return toInt64(personID);
}

qint64 SQLDialog::toInt64(const QVariant& var) {

    bool success = false;

    qint64 int64Value =var.toLongLong(&success);

    if (!success) {

        displayError("Failed to convert the ID to int64");
    }

    Q_ASSERT(int64Value > 0);

    return int64Value;
}

int SQLDialog::pixelWidth(const char text[]) const {

    QFont defaultFont;

    QFontMetrics fm(defaultFont);

    return fm.width(text);
}

const QString SQLDialog::name() const {

    return nameInput->text().toUpper().trimmed();
}

void SQLDialog::displayError(const QString& msg) {

    QMessageBox::critical(this, "Fatal error", msg);
    exit(EXIT_FAILURE);
}

void SQLDialog::displayWarning(const QString& msg) {

    QMessageBox::warning(this, "Error", msg);
}

bool SQLDialog::displayQuestion(const QString& question) {

    int ret = QMessageBox::question(this, "Warning", question, QMessageBox::Yes, QMessageBox::Cancel);

    return (ret == QMessageBox::Yes)? true : false ;
}

void SQLDialog::showEvent(QShowEvent* event) {

    setSelectQueryLikeName();

    QWidget::showEvent(event);
}

void SQLDialog::closeEvent(QCloseEvent* event) {

    clearInput();

    QWidget::closeEvent(event);
}
