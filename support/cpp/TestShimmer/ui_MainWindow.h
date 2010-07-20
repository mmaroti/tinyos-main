/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Thu Jul 15 15:46:30 2010
**      by: Qt User Interface Compiler version 4.6.3
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
    QWidget *plotTab;
    QVBoxLayout *verticalLayout_7;
    QWidget *calibrationTab;
    QVBoxLayout *verticalLayout_2;
    QWidget *consoleTab;
    QVBoxLayout *verticalLayout_4;
    QWidget *plot3dTab;
    QVBoxLayout *verticalLayout_5;
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
        plotTab = new QWidget();
        plotTab->setObjectName(QString::fromUtf8("plotTab"));
        verticalLayout_7 = new QVBoxLayout(plotTab);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        tabWidget->addTab(plotTab, QString());
        calibrationTab = new QWidget();
        calibrationTab->setObjectName(QString::fromUtf8("calibrationTab"));
        verticalLayout_2 = new QVBoxLayout(calibrationTab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabWidget->addTab(calibrationTab, QString());
        consoleTab = new QWidget();
        consoleTab->setObjectName(QString::fromUtf8("consoleTab"));
        verticalLayout_4 = new QVBoxLayout(consoleTab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tabWidget->addTab(consoleTab, QString());
        plot3dTab = new QWidget();
        plot3dTab->setObjectName(QString::fromUtf8("plot3dTab"));
        verticalLayout_5 = new QVBoxLayout(plot3dTab);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tabWidget->addTab(plot3dTab, QString());

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

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TestShimmer", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(connectTab), QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(plotTab), QApplication::translate("MainWindow", "Plot", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(calibrationTab), QApplication::translate("MainWindow", "Calibration", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(consoleTab), QApplication::translate("MainWindow", "Console", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(plot3dTab), QApplication::translate("MainWindow", "3D Plot", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
