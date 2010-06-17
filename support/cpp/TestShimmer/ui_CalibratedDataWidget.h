/********************************************************************************
** Form generated from reading UI file 'CalibratedDataWidget.ui'
**
** Created: Mon Jun 7 16:25:14 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATEDDATAWIDGET_H
#define UI_CALIBRATEDDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "src/PlotScrollArea.h"

QT_BEGIN_NAMESPACE

class Ui_CalibratedDataWidget
{
public:
    QVBoxLayout *verticalLayout;
    PlotScrollArea *scrollArea;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *avgAccel;
    QCheckBox *xyangle;
    QCheckBox *yzangle;
    QCheckBox *zxangle;
    QCheckBox *xAccel;
    QCheckBox *yAccel;
    QCheckBox *zAccel;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *exportButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *CalibratedDataWidget)
    {
        if (CalibratedDataWidget->objectName().isEmpty())
            CalibratedDataWidget->setObjectName(QString::fromUtf8("CalibratedDataWidget"));
        CalibratedDataWidget->resize(508, 389);
        verticalLayout = new QVBoxLayout(CalibratedDataWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scrollArea = new PlotScrollArea(CalibratedDataWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setCursor(QCursor(Qt::CrossCursor));

        verticalLayout->addWidget(scrollArea);

        widget_2 = new QWidget(CalibratedDataWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        avgAccel = new QCheckBox(widget_2);
        avgAccel->setObjectName(QString::fromUtf8("avgAccel"));
        QPalette palette;
        QBrush brush(QColor(85, 173, 176, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        avgAccel->setPalette(palette);
        avgAccel->setChecked(true);

        horizontalLayout_2->addWidget(avgAccel);

        xyangle = new QCheckBox(widget_2);
        xyangle->setObjectName(QString::fromUtf8("xyangle"));
        QPalette palette1;
        QBrush brush2(QColor(14, 0, 120, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        xyangle->setPalette(palette1);
        xyangle->setChecked(true);

        horizontalLayout_2->addWidget(xyangle);

        yzangle = new QCheckBox(widget_2);
        yzangle->setObjectName(QString::fromUtf8("yzangle"));
        QPalette palette2;
        QBrush brush3(QColor(70, 220, 44, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        yzangle->setPalette(palette2);
        yzangle->setChecked(true);

        horizontalLayout_2->addWidget(yzangle);

        zxangle = new QCheckBox(widget_2);
        zxangle->setObjectName(QString::fromUtf8("zxangle"));
        QPalette palette3;
        QBrush brush4(QColor(135, 5, 5, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        zxangle->setPalette(palette3);
        zxangle->setChecked(true);

        horizontalLayout_2->addWidget(zxangle);

        xAccel = new QCheckBox(widget_2);
        xAccel->setObjectName(QString::fromUtf8("xAccel"));
        QFont font;
        font.setPointSize(8);
        xAccel->setFont(font);
        xAccel->setChecked(true);

        horizontalLayout_2->addWidget(xAccel);

        yAccel = new QCheckBox(widget_2);
        yAccel->setObjectName(QString::fromUtf8("yAccel"));
        QPalette palette4;
        QBrush brush5(QColor(0, 0, 255, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush5);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush5);
        QBrush brush6(QColor(118, 116, 108, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush6);
        yAccel->setPalette(palette4);
        yAccel->setChecked(true);

        horizontalLayout_2->addWidget(yAccel);

        zAccel = new QCheckBox(widget_2);
        zAccel->setObjectName(QString::fromUtf8("zAccel"));
        QPalette palette5;
        QBrush brush7(QColor(255, 0, 0, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush7);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush7);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush6);
        zAccel->setPalette(palette5);
        zAccel->setChecked(true);

        horizontalLayout_2->addWidget(zAccel);


        verticalLayout->addWidget(widget_2);

        widget = new QWidget(CalibratedDataWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        exportButton = new QPushButton(widget);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        horizontalLayout->addWidget(exportButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        verticalLayout->setStretch(0, 1);

        retranslateUi(CalibratedDataWidget);

        QMetaObject::connectSlotsByName(CalibratedDataWidget);
    } // setupUi

    void retranslateUi(QWidget *CalibratedDataWidget)
    {
        CalibratedDataWidget->setWindowTitle(QApplication::translate("CalibratedDataWidget", "Form", 0, QApplication::UnicodeUTF8));
        avgAccel->setText(QApplication::translate("CalibratedDataWidget", "average-accel", 0, QApplication::UnicodeUTF8));
        xyangle->setText(QApplication::translate("CalibratedDataWidget", "xy-angle", 0, QApplication::UnicodeUTF8));
        yzangle->setText(QApplication::translate("CalibratedDataWidget", "yz-angle", 0, QApplication::UnicodeUTF8));
        zxangle->setText(QApplication::translate("CalibratedDataWidget", "zx-angle", 0, QApplication::UnicodeUTF8));
        xAccel->setText(QApplication::translate("CalibratedDataWidget", "x-accel", 0, QApplication::UnicodeUTF8));
        yAccel->setText(QApplication::translate("CalibratedDataWidget", "y-accel", 0, QApplication::UnicodeUTF8));
        zAccel->setText(QApplication::translate("CalibratedDataWidget", "z-accel", 0, QApplication::UnicodeUTF8));
        exportButton->setText(QApplication::translate("CalibratedDataWidget", "(E)xport", 0, QApplication::UnicodeUTF8));
        exportButton->setShortcut(QApplication::translate("CalibratedDataWidget", "E", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CalibratedDataWidget: public Ui_CalibratedDataWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATEDDATAWIDGET_H
