#include <QtGui/QApplication>
#include "MainWindow.h"
#include "window.h"
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
        MainWindow w;
	w.show();
        //Plot plot;
        //plot.show();
        /*Window window;
        window.resize(window.sizeHint());
        int desktopArea = QApplication::desktop()->width() *
                         QApplication::desktop()->height();
        int widgetArea = window.width() * window.height();
        if (((float)widgetArea / (float)desktopArea) < 0.75f)
            window.show();
        else
            window.showMaximized();*/

	return a.exec();
}





