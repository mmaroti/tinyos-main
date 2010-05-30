/********************************************************************************
** Form generated from reading UI file 'CalibrationWidget.ui'
**
** Created: Tue May 25 16:48:53 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATIONWIDGET_H
#define UI_CALIBRATIONWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalibrationWidget
{
public:
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *stationaryButton;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *startButton;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QPlainTextEdit *plainTextEdit;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QTextBrowser *calibrationResults;

    void setupUi(QWidget *CalibrationWidget)
    {
        if (CalibrationWidget->objectName().isEmpty())
            CalibrationWidget->setObjectName(QString::fromUtf8("CalibrationWidget"));
        CalibrationWidget->setEnabled(true);
        CalibrationWidget->resize(509, 277);
        horizontalLayout_3 = new QHBoxLayout(CalibrationWidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        groupBox = new QGroupBox(CalibrationWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        stationaryButton = new QRadioButton(groupBox);
        stationaryButton->setObjectName(QString::fromUtf8("stationaryButton"));

        verticalLayout->addWidget(stationaryButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        widget = new QWidget(groupBox);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        startButton = new QPushButton(widget);
        startButton->setObjectName(QString::fromUtf8("startButton"));

        horizontalLayout->addWidget(startButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout->addWidget(widget);


        horizontalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(CalibrationWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        plainTextEdit = new QPlainTextEdit(groupBox_2);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setEnabled(false);

        verticalLayout_2->addWidget(plainTextEdit);

        widget_2 = new QWidget(groupBox_2);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pushButton_3 = new QPushButton(widget_2);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        horizontalLayout_2->addWidget(pushButton_3);

        pushButton_2 = new QPushButton(widget_2);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        horizontalLayout_2->addWidget(pushButton_2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout_2->addWidget(widget_2);


        horizontalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(CalibrationWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setEnabled(true);
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        calibrationResults = new QTextBrowser(groupBox_3);
        calibrationResults->setObjectName(QString::fromUtf8("calibrationResults"));

        verticalLayout_3->addWidget(calibrationResults);


        horizontalLayout_3->addWidget(groupBox_3);


        retranslateUi(CalibrationWidget);

        QMetaObject::connectSlotsByName(CalibrationWidget);
    } // setupUi

    void retranslateUi(QWidget *CalibrationWidget)
    {
        CalibrationWidget->setWindowTitle(QApplication::translate("CalibrationWidget", "Form", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("CalibrationWidget", "Modules", 0, QApplication::UnicodeUTF8));
        stationaryButton->setText(QApplication::translate("CalibrationWidget", "Stationary", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("CalibrationWidget", "(S)tart", 0, QApplication::UnicodeUTF8));
        startButton->setShortcut(QApplication::translate("CalibrationWidget", "S", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("CalibrationWidget", "Directions", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("CalibrationWidget", "Previous", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("CalibrationWidget", "Next", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("CalibrationWidget", "Results", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CalibrationWidget: public Ui_CalibrationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATIONWIDGET_H
