
#include "DataRecorder.h"
#include <QtDebug>

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
