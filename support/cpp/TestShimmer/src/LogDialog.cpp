#include "LogDialog.h"
#include <QDateTime>

LogDialog::LogDialog(QWidget *parent): QDialog(parent)
{
        setupUi(this);
        // Set label text
        dateTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));

}


void LogDialog::on_okButton_clicked()
{
        dateTimeLabel->setText("OK Button clicked ...");
}

void LogDialog::on_cancelButton_clicked()
{
    this->close();
}
