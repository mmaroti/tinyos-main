#include <QtCore/QCoreApplication>

#include "Application.h"
#include <QtDebug>
#include <QFile>


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

            QFile f;

            for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
                if( mode == "s" ){
                    qDebug() << application.statCalMod.Calibrate(application.moteDataHolder.mote(i));
                    f.setFileName("../mote"+QString::number(application.moteDataHolder.mote(i)->getMoteID())+"_static_calib.csv");

                    if( !f.open( QIODevice::Append) ){
                      qDebug() << "Failed to create save file!";
                    }
                    QTextStream ts( &f );

                    for(int i = 0; i < application.statCalMod.solutions.size(); i++){
                        ts << application.statCalMod.solutions.at(i) << endl;
                    }
                    for (unsigned int i = 0; i < 3; i++) {
                        ts << application.statCalMod.gyroMinAvgs[i] << endl;
                    }

                    ts.flush();

                }

                if( mode == "t" ){
                    qDebug() << application .turnCalMod.Calibrate(application.moteDataHolder.mote(i), 45);
                    f.setFileName("../mote"+QString::number(application.moteDataHolder.mote(i)->getMoteID())+"_tt_calib.csv");

                    if( !f.open( QIODevice::Append) ){
                      qDebug() << "Failed to create save file!";
                    }
                    QTextStream ts( &f );

                    for(int i = 0; i < application.turnCalMod.solutions.size(); i++){
                        ts << application.turnCalMod.solutions.at(i) << endl;
                    }

                    ts.flush();
                }



                f.close();


            }
        }

    }

    //return a.exec();
}
