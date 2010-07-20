#include <QtGui/QApplication>
#include "MainWindow.h"
#include "Data3DPlot.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
        Plot plot;
        plot.show();
	return a.exec();
}
