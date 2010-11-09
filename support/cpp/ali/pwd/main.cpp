#include <iostream>
#include <cstdlib>
#include "QDir"
#include "QMessageBox"
#include <QtGui/QApplication>
#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    cout << "PWD: " << QDir::currentPath().toStdString() << endl;
    cout << "changing pwd to ./data" << endl;
    bool success = QDir::setCurrent("data");
    if (!success) {
        cout << "Failed to change working directory" << endl;
        QMessageBox mbox;
        mbox.setText("Failed to change working directory");
        mbox.exec();
        exit(1);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
