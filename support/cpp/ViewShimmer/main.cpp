#include <qapplication.h>
#include "Application.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Application application;


    bool ok = app.exec(); 

    application.samplingThread.stop();
    application.samplingThread.wait(1000);

    return ok;
}
