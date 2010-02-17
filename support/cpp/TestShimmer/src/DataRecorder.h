
#include <QVarLengthArray>
#include <QObject>
#include "SerialListener.h"

#ifndef DATARECORDER_H
#define DATARECORDER_H

struct Sample
{
	int time;
	int xAccel;
	int yAccel;
	int zAccel;
	int xGyro;
	int yGyro;
	int zGyro;
	int voltage;
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
	DataRecorder();
	virtual ~DataRecorder();

	const QVarLengthArray<Sample> & getSamples() const {
		return samples;
	}

	void addSample(const Sample & sample) {
		samples.append(sample);
	}

	int size() const {
		return samples.size();
	}

	const Sample & at(int i) const {
		return samples[i];
	}

	int getFirstTime();
	int getLastTime();

signals:
	void sampleAdded();
	void samplesCleared();

public slots:
	void onReceiveMessage(const ActiveMessage & msg);

public:
	void clearMessages();

protected:
	QVarLengthArray<Sample> samples;
};

#endif // DATARECORDER_H
