/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/


#include <assert.h>
#include "DataRecorder.h"
//#include "Application.h"
#include <QtDebug>
#include <qfile.h>
#include <QStringList>
#include <QMessageBox>
#include <math.h>
#include "constants.h"

#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>
#include "signaldata.h"

class DataRecorder::PrivateData
{
public:
    PrivateData():
        boundingRect(1.0, 1.0, -2.0, -2.0) // invalid
    {
        values.reserve(RESERVED_SAMPLES);
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


    QReadWriteLock lock;

    QVector<QPointF> values;
    QRectF boundingRect;

    QMutex mutex; // protecting pendingValues
    QVector<QPointF> pendingValues;

};

//DataRecorder::DataRecorder(Application &app) : application(app)
DataRecorder::DataRecorder()
{
    d_data = new PrivateData();
    MoteData moteData;
    motes.append(moteData);
}

DataRecorder::~DataRecorder()
{
    delete d_data;
}



void DataRecorder::onReceiveMessage(const ActiveMessage & msg)
{
    // first type of messages
    if( msg.type == 0x37 && msg.payload.size() == 100 )
    {

            for(int start = 0; start < 100; start += 20)
            {
                    Sample sample;

                    sample.time = msg.getInt(start);
                    sample.xAccel = msg.getShort(start + 4);
                    sample.yAccel = msg.getShort(start + 6);
                    sample.zAccel = msg.getShort(start + 8);
                    sample.xGyro = msg.getShort(start + 10);
                    sample.yGyro = msg.getShort(start + 12);
                    sample.zGyro = msg.getShort(start + 14);
                    sample.voltage = msg.getShort(start + 16);
                    sample.temp = msg.getShort(start + 18);

    //			qDebug() << "sample " + sample.toString();

                    /*if(samples.size() > 2){
                        int lag = getLag(sample.time);
                        if(lag > MAX_DUMMY) lag = MAX_DUMMY;
                        for(int i=lag; i > 0; i--){
                            Sample dummy;
                            samples.append(dummy);
                        }
                    }*/
                    motes[0].addSample(sample);
                    qDebug() << "Samples: " << motes[0].size();

                    double time = motes[0].size()/C_HZ;
                    double value = sample.xAccel/16;
                    QPointF f(time, value);
                    //d_data->append(f);
                    SignalData::instance().append(f);
                    qDebug() << "SignalData: " << "x: " << f.x() << "y: " << f.y();
            }
            emit sampleAdded();
    }
}

Sample::Sample()
{
        time = 0;
        xAccel = 0;
        yAccel = 0;
        zAccel = 0;
        xGyro = 0;
        yGyro = 0;
        zGyro = 0;
        voltage = 0;
        temp = 0;
}

int DataRecorder::size() const
{
    return d_data->values.size();
}

QPointF DataRecorder::value(int index) const
{
    return d_data->values[index];
}

QRectF DataRecorder::boundingRect() const
{
    return d_data->boundingRect;
}

DataRecorder *DataRecorder::instance()
{
    //static DataRecorder valueVector;
    return this;
}

void DataRecorder::clearStaleValues(double limit)
{
    d_data->lock.lockForWrite();

    d_data->boundingRect = QRectF(1.0, 1.0, -2.0, -2.0); // invalid

    const QVector<QPointF> values = d_data->values;
    d_data->values.clear();
    d_data->values.reserve(values.size());

    int index;
    for ( index = values.size() - 1; index >= 0; index-- )
    {
        if ( values[index].x() < limit )
            break;
    }

    if ( index > 0 )
        d_data->append(values[index++]);

    while ( index < values.size() - 1 )
        d_data->append(values[index++]);

    d_data->lock.unlock();

}

void DataRecorder::append(const QPointF &sample)
{
    d_data->mutex.lock();
    d_data->pendingValues += sample;

    const bool isLocked = d_data->lock.tryLockForWrite();
    if ( isLocked )
    {
        const int numValues = d_data->pendingValues.size();
        const QPointF *pendingValues = d_data->pendingValues.data();

        for ( int i = 0; i < numValues; i++ )
            d_data->append(pendingValues[i]);

        d_data->pendingValues.clear();

        d_data->lock.unlock();
    }

    d_data->mutex.unlock();
}

void DataRecorder::lock()
{
    d_data->lock.lockForRead();
}

void DataRecorder::unlock()
{
    d_data->lock.unlock();
}
