#include <QtCore/QCoreApplication>

#include "Application.h"
#include <QtDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Application application;

    if(argc != 3){
        qDebug() << "Wrong argument count.";

        a.quit();
    }

    if(argc == 3){

        QString file = argv[1];
        QString mode = argv[2];

        if ( !file.isEmpty() ) {
            application.moteDataHolder.loadCSVData( file );

            for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
                if( mode == "s" ){
                    qDebug() << application.statCalMod.Calibrate(application.moteDataHolder.mote(i));
                }

                if( mode == "t" ){
                    qDebug() << application .turnCalMod.Calibrate(application.moteDataHolder.mote(i), 45);
                }

            }
        }

    }

    //return a.exec();
}
