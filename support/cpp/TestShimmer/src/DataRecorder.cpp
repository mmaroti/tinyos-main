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

#include "DataRecorder.h"
#include <QtDebug>
#include <qfile.h>
#include <string>
#include <algorithm>
#include <vector>
#include <QStringList>


DataRecorder::DataRecorder()
{
}

DataRecorder::~DataRecorder() {
}

int DataRecorder::getFirstTime()
{
	if( samples.isEmpty() )
                return 0;

	return samples[0].time;
}

int DataRecorder::getLastTime()
{
	int size = samples.size();

	if( size == 0 )
		return 0;

	return samples[size-1].time;
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
                    samples.append(sample);
            }
            emit sampleAdded();
    }
}

void DataRecorder::clearMessages()
{
	samples.clear();
	emit samplesCleared();
}

Sample::Sample()
{
	time = -1;
	xAccel = -1;
	yAccel = -1;
	zAccel = -1;
	xGyro = -1;
	yGyro = -1;
	zGyro = -1;
	voltage = -1;
	temp = -1;
}

QString Sample::toString() const
{
	QString s = "(";

	s += "time:" + QString::number(time);

	if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
		s += " accel: " + QString::number(xAccel)
			+ " " + QString::number(yAccel)
			+ " " + QString::number(zAccel);

	if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
		s += " qyro: " + QString::number(xGyro)
			+ " " + QString::number(yGyro)
			+ " " + QString::number(zGyro);

	if( voltage >= 0 )
		s += " volt:" + QString::number(voltage);

	if( temp >= 0 )
		s += " temp:" + QString::number(temp);

	return s + ')';
}

QString Sample::toCsvString() const
{
        QString s = "";

        s += QString::number(time) + ",";

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
                s += QString::number(temp);

        return s;
}

void DataRecorder::csvToSample( QString str )
{    
        QStringList list = str.split(",");
        QStringListIterator csvIterator(list);
        Sample sample;

        sample.time = csvIterator.next().toInt();
        sample.xAccel = csvIterator.next().toInt();
        sample.yAccel = csvIterator.next().toInt();
        sample.zAccel = csvIterator.next().toInt();
        sample.xGyro = csvIterator.next().toInt();
        sample.yGyro = csvIterator.next().toInt();
        sample.zGyro = csvIterator.next().toInt();
        sample.voltage = csvIterator.next().toInt();
        sample.temp = csvIterator.next().toInt();

        DataRecorder::addSample(sample);

}

void DataRecorder::saveSamples( QString filename )
{
    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "Time,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp" << endl;
    for (int i=0; i<samples.size(); i++){
      ts << samples[i].toCsvString() << endl;
    }
    ts.flush();
    //f.close();

}

void DataRecorder::loadSamples( QString filename )
{
    int sorok = 1;
    clearMessages();
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine(); //skip first line of csv
        line = ts.readLine();
        while ( !line.isEmpty() && line != "===CALIBRATED DATA===" ) {
            sorok++;
          csvToSample(line);            //convert line string to sample
          line = ts.readLine();         // line of text excluding '\n'          
        }
        // Close the file
        f.close();
    }
    emit sampleAdded();
}
