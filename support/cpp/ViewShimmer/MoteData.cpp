#include "MoteData.h"

#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>
#include <QtDebug>
#include <qwt_series_data.h>
#include "constants.h"

class MoteData::PrivateData
{
public:
    PrivateData():
        boundingRect(1.0, 1.0, -2.0, -2.0) // invalid
    {
        //values.reserve(RESERVED_SAMPLES);
    }

    inline void append(const QPointF &sample)
    {
        values.append(sample);

        // adjust the bounding rectangle

        if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
        {
            boundingRect.setRect(sample.x(), sample.y(), 0.0, 0.0);
        }
        else
        {
            boundingRect.setRight(sample.x());

            if ( sample.y() > boundingRect.bottom() )
                boundingRect.setBottom(sample.y());

            if ( sample.y() < boundingRect.top() )
                boundingRect.setTop(sample.y());
        }
    }


    QVector<QPointF> values;
    QRectF boundingRect;

};

Sample::Sample()
{
    moteID = 0;
    mote_time = 0;
    counter = 0;
    unix_time = 0;
    xAccel = 0;
    yAccel = 0;
    zAccel = 0;
    xGyro = 0;
    yGyro = 0;
    zGyro = 0;
    voltage = 0;
    temp = 0;
}

MoteData::MoteData()
{
    //samples.reserve(RESERVED_SAMPLES);
    d_data = new PrivateData();
}

MoteData::~MoteData()
{
    //samples.clear();
    delete d_data;
}

void MoteData::setParam(Params param, int value)
{
    if(param == MOTEID){
        moteID = value;
    } else if(param == REBOOTID){
        rebootID = value;
    }
}

void MoteData::setParam(Params param, double value)
{
    if(param == LENGTH){
        length = value;
    } else if(param == SKEW_1){
        skew_1 = value;
    } else if(param == OFFSET){
        offset = value;
    }
}

void MoteData::setBootUnixTime(double value)
{
    boot_unix_time = value;
}

int MoteData::size() const
{
    return d_data->values.size();
}

QPointF MoteData::value(int index) const
{
    return d_data->values[index];
}

QRectF MoteData::boundingRect() const
{
    return d_data->boundingRect;
}


void MoteData::append(const QPointF &sample)
{

    d_data->append(sample);
}


MoteData &MoteData::instance()
{
    static MoteData valueVector;
    return valueVector;
}

QString Sample::toCsvString() const
{
    QString s = "";

    s += QString::number(unix_time) + ",";
    s += QString::number(mote_time) + ",";
    s += QString::number(counter) + ",";

    if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
            s += QString::number(xAccel)
                    + "," + QString::number(yAccel)
                    + "," + QString::number(zAccel)
                    + ",";

    if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
            s += QString::number(xGyro)
                    + "," + QString::number(yGyro)
                    + "," + QString::number(zGyro)
                    + ",";;

    if( voltage >= 0 )
            s += QString::number(voltage) + ",";

    if( temp >= 0 )
            s += QString::number(temp) + ",";

    return s;
}

QString MoteData::getMoteHeader()
{
    QString header;

    header.append(QString::number(moteID)+",");
    header.append(QString::number(rebootID)+",");
    header.append(QString::number(length)+",");
    header.append(QString::number(boot_unix_time)+",");
    header.append(QString::number(skew_1)+",");
    header.append(QString::number(offset)+",");

    return header;

}

void MoteData::insertBlankSamples(int i, int x)
{

    Sample sample;
    samples.insert(i,x,sample);

}

void MoteData::setTimeDelay(double delay, int from)
{

    for(int i = from; i < samples.size(); i++){
        getSampleAt(i).unix_time = samples.at(i).unix_time + delay;
    }

    length += delay;
}

void MoteData::deleteSamplesFrom(int i, int count)
{
    double timeRemove;
    timeRemove = samples.at(i+count).unix_time - samples.at(i).unix_time;
    qDebug() << "Length = " << length << "-" << timeRemove << "=" << length-timeRemove;
    length -= timeRemove;

    samples.remove(i, count);
}
