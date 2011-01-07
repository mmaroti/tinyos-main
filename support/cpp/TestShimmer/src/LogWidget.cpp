#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "Application.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDateTime>

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app)
{
    ui->setupUi(this);

    ui->log->setRowCount(1);
    ui->entryLine->setFocus();
}

LogWidget::~LogWidget()
{

}


void LogWidget::on_entryLine_editingFinished()
{
    if(!ui->entryLine->text().isNull()){
        createItem(ui->entryLine->text());
    }
}

void LogWidget::createItem(QString text)
{
    int row = ui->log->rowCount();
    ui->log->setRowCount(row+1);

    QPushButton* but = new QPushButton("goto",this);
    ui->log->setCellWidget(row-1,0,but);
    QTableWidgetItem* item2 = new QTableWidgetItem(QDateTime::currentDateTime().toString(),1);
    ui->log->setItem(row-1,1,item2);
    QTableWidgetItem* item = new QTableWidgetItem(text,1);
    ui->log->setItem(row-1,2,item);

    ui->entryLine->clear();
    ui->entryLine->setFocus();
}
