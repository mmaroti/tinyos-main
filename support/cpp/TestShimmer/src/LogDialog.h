#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include "ui_LogDialog.h"

class LogDialog : public QDialog, private Ui::LogDialog
{
                Q_OBJECT

        private slots:

                void on_okButton_clicked();
                void on_cancelButton_clicked();
        public:
                LogDialog(QWidget *parent = 0);
                ~LogDialog(){}
};


#endif // LOGDIALOG_H
