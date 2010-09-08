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

#include <QVarLengthArray>
#include <QObject>
#include "Data.hpp"
#include "SerialListener.h"

#ifndef DATARECORDER_H
#define DATARECORDER_H

class Application;

struct Sample
{
	Sample();
	QString toString() const;
        QString toCsvString() const;

	int time;
	int xAccel;
	int yAccel;
	int zAccel;
	int xGyro;
	int yGyro;
	int zGyro;
	int voltage;
	int temp;
        double XYangle, YZangle, ZXangle;
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
        DataRecorder(Application &application);
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

        Sample & setAngle(int i) {
            return samples[i];
        }

	const Sample & at(int i) const {
		return samples[i];
	}

	int getFirstTime();
	int getLastTime();

        void saveCalibrationData();
        void loadCalibrationData();

        double* getAccelCalibration() { return accelCalibrationData; }
        double* getGyroCalibration() { return gyroCalibrationData; }
        double* getGyroMinAvgs() { return gyroMinAvgs; }
        int* getAccelIdleWindowStart() { return accelIdleWindowStart; }

        void at(int i, double data[SIZE]) const;

signals:
	void sampleAdded();
	void samplesCleared();

public slots:
	void onReceiveMessage(const ActiveMessage & msg);

public:
	void clearMessages();
        void saveSamples(QString);
        void loadSamples(QString);
        void csvToSample(QString);

protected:
	QVarLengthArray<Sample> samples;
        double accelCalibrationData[12];
        double gyroCalibrationData[12];
        double gyroMinAvgs[3];
        int accelIdleWindowStart[6];

private:
        Application &application;

};
#endif // DATARECORDER_H
