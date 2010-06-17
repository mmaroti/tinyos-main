#include "Application.h"
#include "ConsoleWidget.h"
#include "ui_ConsoleWidget.h"
#include <qtextedit.h>
#include <qtextstream.h>
#include <qfile.h>
#include <iostream>
#include <qiodevice.h>
#include <QtDebug>

ConsoleWidget::ConsoleWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::ConsoleWidget),
    application(app)
{
    ui->setupUi(this);

}

ConsoleWidget::~ConsoleWidget()
{
    delete ui;
}

void ConsoleWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConsoleWidget::onRecieveConsoleSignal(QString msg) {
    ui->textEdit->setText(msg);
}
