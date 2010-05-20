/********************************************************************************
** Form generated from reading UI file 'ConsoleWidget.ui'
**
** Created: Tue May 18 15:47:08 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLEWIDGET_H
#define UI_CONSOLEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConsoleWidget
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;

    void setupUi(QWidget *ConsoleWidget)
    {
        if (ConsoleWidget->objectName().isEmpty())
            ConsoleWidget->setObjectName(QString::fromUtf8("ConsoleWidget"));
        ConsoleWidget->resize(400, 300);
        verticalLayout = new QVBoxLayout(ConsoleWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit = new QTextEdit(ConsoleWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setReadOnly(true);

        verticalLayout->addWidget(textEdit);


        retranslateUi(ConsoleWidget);

        QMetaObject::connectSlotsByName(ConsoleWidget);
    } // setupUi

    void retranslateUi(QWidget *ConsoleWidget)
    {
        ConsoleWidget->setWindowTitle(QApplication::translate("ConsoleWidget", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConsoleWidget: public Ui_ConsoleWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLEWIDGET_H
