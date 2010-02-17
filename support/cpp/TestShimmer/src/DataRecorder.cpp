
#include "DataRecorder.h"

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
	// TODO: parse message

	Sample sample;

	sample.time = getLastTime() + 128;
	sample.xAccel = qrand() % 4096;
	sample.voltage = 2000;

	samples.append(sample);
	emit sampleAdded();
}

void DataRecorder::clearMessages()
{
	samples.clear();
	emit samplesCleared();
}
