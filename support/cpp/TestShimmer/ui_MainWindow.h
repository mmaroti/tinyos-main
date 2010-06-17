/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Mon Jun 7 16:25:14 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QTabWidget *tabWidget;
    QWidget *connectTab;
    QVBoxLayout *verticalLayout_3;
    QWidget *rawTab;
    QVBoxLayout *verticalLayout;
    QWidget *calibratedTab;
    QVBoxLayout *verticalLayout_6;
    QWidget *calibrationTab;
    QVBoxLayout *verticalLayout_2;
    QWidget *consoleTab;
    QVBoxLayout *verticalLayout_4;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(568, 360);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/Wireless.png"), QSize(), QIcon::Normal, QIcon::On);
        MainWindow->setWindowIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 0);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        connectTab = new QWidget();
        connectTab->setObjectName(QString::fromUtf8("connectTab"));
        verticalLayout_3 = new QVBoxLayout(connectTab);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        tabWidget->addTab(connectTab, QString());
        rawTab = new QWidget();
        rawTab->setObjectName(QString::fromUtf8("rawTab"));
        verticalLayout = new QVBoxLayout(rawTab);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget->addTab(rawTab, QString());
        calibratedTab = new QWidget();
        calibratedTab->setObjectName(QString::fromUtf8("calibratedTab"));
        verticalLayout_6 = new QVBoxLayout(calibratedTab);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        tabWidget->addTab(calibratedTab, QString());
        calibrationTab = new QWidget();
        calibrationTab->setObjectName(QString::fromUtf8("calibrationTab"));
        verticalLayout_2 = new QVBoxLayout(calibrationTab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabWidget->addTab(calibrationTab, QString());
        consoleTab = new QWidget();
        consoleTab->setObjectName(QString::fromUtf8("consoleTab"));
        verticalLayout_4 = new QVBoxLayout(consoleTab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tabWidget->addTab(consoleTab, QString());

        horizontalLayout_2->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 568, 21));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TestShimmer", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(connectTab), QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(rawTab), QApplication::translate("MainWindow", "Raw data", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(calibratedTab), QApplication::translate("MainWindow", "Acceleration", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(calibrationTab), QApplication::translate("MainWindow", "Calibration", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(consoleTab), QApplication::translate("MainWindow", "Console", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
