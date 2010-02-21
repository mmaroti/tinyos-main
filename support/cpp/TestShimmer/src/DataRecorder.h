
#include <QQueue>
#include <QObject>
#include "SerialListener.h"

#ifndef DATARECORDER_H
#define DATARECORDER_H

struct Sample
{
	QString toString() const;

	int time;
	int xAccel;
	int yAccel;
	int zAccel;
	int xGyro;
	int yGyro;
	int zGyro;
	int voltage;
	int temp;
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
	DataRecorder();
	virtual ~DataRecorder();

	const QQueue<Sample>& getSamples() const {
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
	QQueue<Sample> samples;
};

// Make Qt aware of the Sample type
Q_DECLARE_TYPEINFO(Sample, Q_PRIMITIVE_TYPE);

#endif // DATARECORDER_H
