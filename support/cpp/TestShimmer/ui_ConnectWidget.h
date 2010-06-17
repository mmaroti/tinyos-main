/********************************************************************************
** Form generated from reading UI file 'ConnectWidget.ui'
**
** Created: Mon Jun 7 16:25:14 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTWIDGET_H
#define UI_CONNECTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConnectWidget
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout;
    QListWidget *motes;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *refreshButton;
    QSpacerItem *horizontalSpacer_4;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *baudRate;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QGroupBox *comPorts;
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *ConnectWidget)
    {
        if (ConnectWidget->objectName().isEmpty())
            ConnectWidget->setObjectName(QString::fromUtf8("ConnectWidget"));
        ConnectWidget->resize(462, 308);
        gridLayout = new QGridLayout(ConnectWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox = new QGroupBox(ConnectWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        hboxLayout = new QHBoxLayout(groupBox);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        motes = new QListWidget(groupBox);
        motes->setObjectName(QString::fromUtf8("motes"));
        motes->setFrameShape(QFrame::Box);
        motes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        motes->setSpacing(2);

        hboxLayout->addWidget(motes);


        gridLayout->addWidget(groupBox, 0, 2, 1, 1);

        widget = new QWidget(ConnectWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        refreshButton = new QPushButton(widget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));

        horizontalLayout->addWidget(refreshButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        gridLayout->addWidget(widget, 1, 2, 1, 1);

        widget_2 = new QWidget(ConnectWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        baudRate = new QComboBox(widget_2);
        baudRate->insertItems(0, QStringList()
         << QString::fromUtf8("57600")
         << QString::fromUtf8("115200")
         << QString::fromUtf8("230400")
         << QString::fromUtf8("460800")
         << QString::fromUtf8("921600")
        );
        baudRate->setObjectName(QString::fromUtf8("baudRate"));

        horizontalLayout_2->addWidget(baudRate);

        connectButton = new QPushButton(widget_2);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        horizontalLayout_2->addWidget(connectButton);

        disconnectButton = new QPushButton(widget_2);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));

        horizontalLayout_2->addWidget(disconnectButton);


        gridLayout->addWidget(widget_2, 1, 1, 1, 1);

        comPorts = new QGroupBox(ConnectWidget);
        comPorts->setObjectName(QString::fromUtf8("comPorts"));
        verticalLayout = new QVBoxLayout(comPorts);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        gridLayout->addWidget(comPorts, 0, 1, 1, 1);

        gridLayout->setRowStretch(0, 1);

        retranslateUi(ConnectWidget);

        baudRate->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(ConnectWidget);
    } // setupUi

    void retranslateUi(QWidget *ConnectWidget)
    {
        ConnectWidget->setWindowTitle(QApplication::translate("ConnectWidget", "Form", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ConnectWidget", "Motes", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("ConnectWidget", "(R)efresh", 0, QApplication::UnicodeUTF8));
        refreshButton->setShortcut(QApplication::translate("ConnectWidget", "R", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ConnectWidget", "Baudrate:", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("ConnectWidget", "(C)onnect", 0, QApplication::UnicodeUTF8));
        connectButton->setShortcut(QApplication::translate("ConnectWidget", "C", 0, QApplication::UnicodeUTF8));
        disconnectButton->setText(QApplication::translate("ConnectWidget", "(D)isconnect", 0, QApplication::UnicodeUTF8));
        disconnectButton->setShortcut(QApplication::translate("ConnectWidget", "D", 0, QApplication::UnicodeUTF8));
        comPorts->setTitle(QApplication::translate("ConnectWidget", "COM ports", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ConnectWidget: public Ui_ConnectWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTWIDGET_H
