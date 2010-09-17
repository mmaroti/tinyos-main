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

#include <stdexcept>
#include "DataRecorder.h"
#include "Application.h"
#include <QtDebug>
#include <qfile.h>
#include <string>
#include <algorithm>
#include <vector>
#include <QStringList>
#include <QMessageBox>
#include "DataPlot.h"

DataRecorder::DataRecorder(Application &app) : application(app)
{
    //loadCalibrationData();
    for(int i=0; i<6; i++){
        accelIdleWindowStart[i] = -1;
    }
    for(int i=0; i<7; i++){
        gyroIdleWindowStart[i] = -1;
    }

    from = to = 0;
    copySamples.clear();
}

DataRecorder::~DataRecorder() {
}

int DataRecorder::getFirstTime() const {

	if( samples.isEmpty() )
                return 0;

	return samples[0].time;
}

int DataRecorder::getLastTime() const {

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
        XYangle = 0.0;
        YZangle = 0.0;
        ZXangle = 0.0;
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

void DataRecorder::csvToSample(const QString& str)
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

        samples.append(sample);

}

void DataRecorder::saveSamples(const QString& filename) const {

    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#Time,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp" << endl;
    for (int i=0; i<samples.size(); i++){
      ts << samples[i].toCsvString() << endl;
    }
    ts.flush();
    //f.close();

}

void DataRecorder::loadSamples(const QString& filename )
{
    clearMessages();
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine(); //skip first line of csv

        if(line[0] != QChar('#')){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            line = ts.readLine();
            while ( !line.isEmpty() && line != "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" )
            {
                csvToSample(line);            //convert line string to sample
                line = ts.readLine();         // line of text excluding '\n'
            }
            f.close();
        }
    }
    emit sampleAdded();
}

void DataRecorder::loadCalibrationData()
{
    int size = application.settings.beginReadArray("stationaryCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        accelCalibrationData[i] = application.settings.value("stationaryCalibrationData").toDouble();
    }
    application.settings.endArray();

    size = application.settings.beginReadArray("gyroCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        gyroCalibrationData[i] = application.settings.value("gyroCalibrationData").toDouble();
    }
    application.settings.endArray();

    size = application.settings.beginReadArray("gyroAvgsData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        gyroMinAvgs[i] = application.settings.value("gyroAvgsData").toDouble();
    }
    application.settings.endArray();
}

void DataRecorder::saveCalibrationData() const
{
    application.settings.beginWriteArray("stationaryCalibrationData");
    for (unsigned int i = 0; i < 12; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("stationaryCalibrationData", accelCalibrationData[i]);
    }
    application.settings.endArray();

    application.settings.beginWriteArray("gyroCalibrationData");
    for (unsigned int i = 0; i < 12; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("gyroCalibrationData", gyroCalibrationData[i]);
    }
    application.settings.endArray();

    application.settings.beginWriteArray("gyroAvgsData");
    for (unsigned int i = 0; i < 3; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("gyroAvgsData", gyroMinAvgs[i]);
    }
    application.settings.endArray();

}

void DataRecorder::at(int i, double data[ipo::SIZE]) const {

    using namespace ipo;

    if (i<0 || i>=application.dataRecorder.size()) {

        throw std::out_of_range("Index out of range!");
    }

    const Sample& s = samples[i];

    data[TIME_STAMP] = s.time;

    const double ax = s.xAccel;
    const double ay = s.yAccel;
    const double az = s.zAccel;

    data[ACCEL_X] = ax*accelCalibrationData[0] + ay*accelCalibrationData[1] + az*accelCalibrationData[2] + accelCalibrationData[9];
    data[ACCEL_Y] = ax*accelCalibrationData[3] + ay*accelCalibrationData[4] + az*accelCalibrationData[5] + accelCalibrationData[10];
    data[ACCEL_Z] = ax*accelCalibrationData[6] + ay*accelCalibrationData[7] + az*accelCalibrationData[8] + accelCalibrationData[11];

    const double wx = s.xGyro;
    const double wy = s.yGyro;
    const double wz = s.zGyro;

    data[GYRO_X] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[0] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[1] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[2];
    data[GYRO_Y] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[3] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[4] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[5];
    data[GYRO_Z] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[6] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[7] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[8];
}

void DataRecorder::edit(QString option)
{
    if(samples.size()< to){
        to = samples.size()-1;
    }

    if(option == "trim"){
        for(int i=from; i<=to; i++){
            copySamples.append(samples[i]);
        }
        samples.clear();
        emit samplesCleared();
        samples = copySamples;
        emit sampleAdded();
        copySamples.clear();
    } else
    if(option == "copy"){
        for(int i=from; i<=to; i++){            
            copySamples.append(samples[i]);
        }
        for(int i=0; i<copySamples.size(); i++){
            samples.append(copySamples[i]);
        }
        copySamples.clear();
        emit sampleAdded();
    }
}
