#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "Application.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDateTime>
#include <QDateTimeEdit>

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app)
{
    ui->setupUi(this);

    signalMapper = new QSignalMapper(this);

    ui->log->setRowCount(1);
    ui->log->horizontalHeader()->resizeSection(0, 40);
    ui->log->horizontalHeader()->resizeSection(1, 110);
    ui->log->horizontalHeader()->resizeSection(2, 70);
    ui->log->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
    ui->log->horizontalHeader()->resizeSection(4, 40);

    ui->log->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->entryLine->setFocus();

    connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(onDelRow(int)));
}

LogWidget::~LogWidget()
{

}


void LogWidget::on_entryLine_returnPressed()
{
    if(!ui->entryLine->text().isNull()){
        createItem(ui->entryLine->text());
    }
}

void LogWidget::createItem(QString text)
{
    int row = ui->log->rowCount();
    ui->log->setRowCount(row+1);

    QPushButton* but = new QPushButton(QIcon(":/icons/back-arrow.png"),"",this);
    //but->setMaximumSize(40,20);
    ui->log->setCellWidget(row-1,0,but);

    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
     dateEdit->setMinimumDate(QDate::currentDate().addDays(-365));
     dateEdit->setMaximumDate(QDate::currentDate().addDays(365));
     dateEdit->setCalendarPopup(true);
     //dateEdit->setReadOnly(true);
     dateEdit->setDisplayFormat("yyyy.MM.dd");
    ui->log->setCellWidget(row-1, 1, dateEdit);
    ui->log->cellWidget(row-1, 1)->setEnabled(false);

    QDateTimeEdit *time = new QDateTimeEdit(QTime::currentTime());
     //time->setReadOnly(true);
    ui->log->setCellWidget(row-1, 2, time);
    ui->log->cellWidget(row-1, 2)->setEnabled(false);

    QTableWidgetItem* item = new QTableWidgetItem(text,1);
    ui->log->setItem(row-1,3,item);

    QPushButton* del = new QPushButton(QIcon(":/icons/Delete.png"),"",this);
     del->setMaximumSize(20,20);
    ui->log->setCellWidget(row-1,4,del);
    signalMapper->setMapping(del, row-1);

    connect(del, SIGNAL(clicked()), signalMapper, SLOT (map()));

    ui->entryLine->clear();
    ui->entryLine->setFocus();
}

void LogWidget::setTableEditable(bool isEditable)
{
    //ui->log->setEnabled(isEditable);
    if(isEditable){        
        ui->log->setEditTriggers(QAbstractItemView::DoubleClicked);
        for(int i = 0; i<ui->log->rowCount()-1; i++){
            ui->log->cellWidget(i,1)->setEnabled(true);
            ui->log->cellWidget(i,2)->setEnabled(true);
        }
    } else {
        ui->log->setEditTriggers(QAbstractItemView::NoEditTriggers);
        for(int i = 0; i<ui->log->rowCount()-1; i++){
            ui->log->cellWidget(i,1)->setEnabled(false);
            ui->log->cellWidget(i,2)->setEnabled(false);
        }
    }
}

void LogWidget::on_felvKezdButton_clicked()
{
    createItem(QString::fromUtf8("Felvétel Kezdete"));
}

void LogWidget::on_felvVegButton_clicked()
{
    createItem(QString::fromUtf8("Felvétel Vége"));
}

void LogWidget::on_mozgKezdButton_clicked()
{
    createItem(QString::fromUtf8("Mozgás Kezdete"));
}

void LogWidget::on_mozgVegButton_clicked()
{
    createItem(QString::fromUtf8("Mozgás Vége"));
}

void LogWidget::onDelRow(int row)
{
    ui->log->removeRow(row);
}
