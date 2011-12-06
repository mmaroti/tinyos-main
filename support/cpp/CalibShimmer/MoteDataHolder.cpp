#include "MoteDataHolder.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QStringListIterator>
#include <QtDebug>
#include <QDateTime>
#include <QTime>
#include <math.h>

MoteDataHolder::MoteDataHolder()
{
}

MoteDataHolder::~MoteDataHolder()
{
    for(int i = 0; i < motes.size(); i++) delete motes[i];
}

void MoteDataHolder::loadCSVData(QString filename)
{
    QFile f( filename );
    QString line;

    qDebug() << f.fileName();

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine();

        if(line != "#mote,reboot_ID,length,boot_unix_time,skew_1,offset"){
            qDebug() << "Wrong file format! Wrong header!";
            qDebug() << line;
        } else {
            line = ts.readLine();

            while ( !line.isEmpty() && line != "#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp" )
            {
                createMoteDataFromCSV(line);            //convert line string to mote header data
                line = ts.readLine();         // line of text excluding '\n'
            }

            //printMotesHeader();

            //skip empty lines
            while( line != "#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp" )
            {
                line = ts.readLine();
            }

            line = ts.readLine();

            while ( !line.isEmpty() && line != "#marker_id,marker_text,marker_x_pos" )
            {
                createSample(line, true);          //convert line string to sample
                line = ts.readLine();         // line of text excluding '\n'
            }

            f.close();
        }
    }

    //printMoteData(4);
    qDebug()  << "-----loading finished-----";
    qDebug() << motes.size();
    for(int i=0; i<motes.size(); i++){
        qDebug() << motes[i]->samplesSize();
    }
    //emit loadFinished();
}


void MoteDataHolder::createMoteDataFromCSV(const QString& line)
{
    QStringList list = line.split(",");
    QStringListIterator csvIterator(list);

    int moteID = csvIterator.next().toInt();


    MoteData* moteData;
    if ( getMoteData(moteID) != NULL ){
        return;
    } else {
        moteData = new MoteData();
    }

    if(csvIterator.hasNext()){
        moteData->setParam(MOTEID, moteID);
        moteData->setParam(REBOOTID, csvIterator.next().toInt());
        moteData->setParam(LENGTH, csvIterator.next().toDouble());        
        moteData->setParam(BOOT_UNIX_TIME, csvIterator.next().toDouble());
        moteData->setParam(SKEW_1, csvIterator.next().toDouble());
        moteData->setParam(OFFSET, csvIterator.next().toDouble());
    }



    //int prevMax = progressBar->maximum();
    //int length = moteData->getLength()*204.8;
    //progressBar->setMaximum(prevMax+length);

    motes.append(moteData);
}

const Sample MoteDataHolder::createSample(const QString& str, bool load)
{
    QStringList list = str.split(",");
    QStringListIterator csvIterator(list);

    Sample sample;    
    MoteData* mote;

    int moteID;
    int counter;

    if(csvIterator.hasNext()){
        sample.moteID = moteID = csvIterator.next().toInt();
        mote = getMoteData(moteID);

        int reboot_ID = csvIterator.next().toInt();

        sample.unix_time = csvIterator.next().toDouble();

        int mote_time = csvIterator.next().toInt();
        counter = csvIterator.next().toInt();

        sample.xAccel = csvIterator.next().toInt();
        sample.yAccel = csvIterator.next().toInt();
        sample.zAccel = csvIterator.next().toInt();
        sample.xGyro = csvIterator.next().toInt();
        sample.yGyro = csvIterator.next().toInt();
        sample.zGyro = csvIterator.next().toInt();
        sample.voltage = csvIterator.next().toInt();
        sample.temp = csvIterator.next().toInt();
    }

    if(load) mote->appendSample(sample);

    return sample;

//    double time = sample.unix_time;
//    double value = (sample.xAccel-2300)/12;
//    QPointF point(time, value);
//    MoteData::instance().append(point);

}

int MoteDataHolder::findMotePos(const MoteData &moteData)
{
    for(int i=0; i<motes.size(); i++){
        if(motes[i] == &moteData){
            return i;
        }
    }
    return -1;
}

MoteData* MoteDataHolder::getMoteData(int id)
{
    for(int i=0; i<motes.size(); i++){
        if(motes[i]->getMoteID() == id){
            return motes[i];
        }
    }
    return NULL;
}

int MoteDataHolder::findNearestSample(double time, int mote)
{
    int pos = time / 0.005;

    //absolut erteket figyelni...
    //time lehet negativ is... TODO
    if(pos < 0) pos = 0;
    if(pos > motes[mote]->samplesSize()) pos = motes[mote]->samplesSize()-1;
    if(motes[mote]->sampleAt(0).unix_time > time + 0.5) return 0;
    if(motes[mote]->sampleAt(motes[mote]->samplesSize()-1).unix_time < time - 0.5) return motes[mote]->samplesSize()-1;

    double unix_time = motes[mote]->sampleAt(pos).unix_time;
    double diff = fabs(time - unix_time);

    while(diff > 0.005){
        if( unix_time > time){
            pos--;
        } else {
            pos++;
        }

        if( pos <= 0 ) return 0;
        if( pos >= motes[mote]->samplesSize() ) return motes[mote]->samplesSize()-1;

        unix_time = motes[mote]->sampleAt(pos).unix_time;
        diff = fabs(time - unix_time);
    }
    return pos;
}
