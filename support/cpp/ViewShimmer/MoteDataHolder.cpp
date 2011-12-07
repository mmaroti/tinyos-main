#include "MoteDataHolder.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QStringListIterator>
#include <QtDebug>
#include <QDateTime>
#include <QTime>
#include <math.h>
#include "Application.h"

MoteDataHolder::MoteDataHolder(Application &app) : application(app)
{
    progressCounter = 0;
}

MoteDataHolder::~MoteDataHolder()
{
    for(int i = 0; i < motes.size(); i++) delete motes[i];
}

void MoteDataHolder::loadCSVData(QString filename)
{
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine();

        if(line != "#mote,reboot_ID,length,boot_unix_time,skew_1,offset"){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format! Wrong header!");
            msgBox.exec();
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
    for(int i=0; i<motes.size(); i++){
        qDebug() << motes[i]->samplesSize();
    }
    emit loadFinished();
}

void MoteDataHolder::saveData(const QString& filename) const {

    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#mote,reboot_ID,length,boot_unix_time,skew_1,offset" << endl;
    for (int i=0; i<motes.size(); i++){
      ts << motes[i]->getMoteHeader() << endl;
    }

    ts << endl;

    ts << "#mote,reboot_ID,unix_time,mote_time,counter,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,volt,temp" << endl;

    for(int i = 0; i < motes.size(); i++){
        for(int j = 0; j < motes[i]->samplesSize(); j++){
            ts << motes[i]->getMoteID() << "," << motes[i]->getRebootID() << "," << motes[i]->sampleAt(j).toCsvString() << endl;
        }
    }

    ts.flush();
    f.close();

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

void MoteDataHolder::createMoteDataFromOnline(int moteID)
{
    MoteData* moteData = new MoteData();

    moteData->setParam(MOTEID, moteID);
    moteData->setParam(REBOOTID, 0);
    moteData->setParam(LENGTH, 0.0);
    moteData->setParam(BOOT_UNIX_TIME, 0.0);
    moteData->setParam(SKEW_1, 0.0);
    moteData->setParam(OFFSET, 0.0);

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

void MoteDataHolder::createSampleFromOnline(const ActiveMessage & msg)
{
    MoteData* mote = getMoteData(msg.source);

    // first type of messages
    if( msg.type == 0x37 && msg.payload.size() == 100 )
    {

            for(int start = 0; start < 100; start += 20)
            {
                    Sample sample;

                    sample.mote_time = msg.getUInt(start);
                    sample.xAccel = msg.getShort(start + 4);
                    sample.yAccel = msg.getShort(start + 6);
                    sample.zAccel = msg.getShort(start + 8);
                    sample.xGyro = msg.getShort(start + 10);
                    sample.yGyro = msg.getShort(start + 12);
                    sample.zGyro = msg.getShort(start + 14);
                    sample.voltage = msg.getShort(start + 16);
                    sample.temp = msg.getShort(start + 18);

                    long long int elapsed_time = application.connect_time.elapsed();

                    long long int mote_time_msec = (sample.mote_time/32768.0)*1000;
                    long long int mote_boot_time = elapsed_time - mote_time_msec;

                    // update boot time to track time sync errors
                    if(mote->samplesSize() == 0)
                        mote->setBootUnixTime(mote_boot_time);
                    else {
                        mote->setBootUnixTime(0.99 * mote->getBootUnixTime()
                                            + 0.01 * mote_boot_time);
                    }

                    sample.unix_time = (mote->getBootUnixTime() + mote_time_msec)/1000.0;

                    //qDebug() << "elapsed: " << elapsed_time << "; mote_time_msec: " << mote_time_msec << "; mote_boot_time: " << mote_boot_time;
                    //qDebug() << "sample.unix_time = " << mote->getBootUnixTime() << " + " << mote_time_msec;

                    /*if(samples.size() > 2){
                        int lag = getLag(sample.time);
                        if(lag > MAX_DUMMY) lag = MAX_DUMMY;
                        for(int i=lag; i > 0; i--){
                            Sample dummy;
                            samples.append(dummy);
                        }
                    }*/
                    mote->appendSample(sample);
            }
            emit sampleAdded(msg.source);
    }
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

void MoteDataHolder::printMoteData(int id)
{
    MoteData* mote = getMoteData(id);

    for(int i = 0; i < mote->samplesSize(); i++){
        qDebug() << QString::number(mote->getSampleAt(i).xAccel);
    }

}

void MoteDataHolder::printMotesHeader()
{
    QString text;
    text.append("===============================================\n");
    for(int i=0; i<motes.size(); i++){
        text.append("Mote: " + QString::number(motes[i]->getMoteID()) + ", ");
        text.append(QString::number(motes[i]->getRebootID()) + ", ");
        text.append(QString::number(motes[i]->getLength()) + ", ");
        text.append(QString::number(motes[i]->getBootUnixTime()) + "\n");

        qDebug() << motes[i]->getMoteID();
        qDebug() << motes[i]->getRebootID();
    }

    text.append("===============================================\n");

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

void MoteDataHolder::calculateOffset(int sample)
{
    QMessageBox msgbox;
    msgbox.setText(QString::number(sample));
    msgbox.exec();

    double offset = -(double)(160*sample)/32768;


    motes[0]->setTimeDelay(offset, 0);

}
