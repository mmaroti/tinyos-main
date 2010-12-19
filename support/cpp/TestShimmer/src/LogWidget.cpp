#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "Application.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>

LogWidget::LogWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::LogWidget),
        application(app)
{
    ui->setupUi(this);

    ui->log->setRowCount(10);

    QTableWidgetItem* item = new QTableWidgetItem("valami",1);
    QPushButton* but = new QPushButton("goto",this);
    ui->log->setCellWidget(0,0,but);
    ui->log->setItem(0,2,item);
    QTableWidgetItem* item2 = new QTableWidgetItem("2010-12-10 11:14",1);
    ui->log->setItem(0,1,item2);
}

LogWidget::~LogWidget()
{

}
