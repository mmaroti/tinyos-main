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
        Sample(); // FIXME
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
        double XYangle, YZangle, ZXangle; // FIXME
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
        DataRecorder(Application &application);
        ~DataRecorder();

	int size() const {
		return samples.size();
	}

        Sample & setAngle(int i) { // FIXME
            return samples[i];
        }

	const Sample & at(int i) const {
		return samples[i];
	}

        int getFirstTime() const;
        int getLastTime() const;

        void saveCalibrationData() const;
        void loadCalibrationData();

        double* getAccelCalibration() { return accelCalibrationData; } // FIXME
        double* getGyroCalibration() { return gyroCalibrationData; }
        double* getGyroMinAvgs() { return gyroMinAvgs; }
        int* getAccelIdleWindowStart() { return accelIdleWindowStart; }
        int* getGyroIdleWindowStart() { return gyroIdleWindowStart; }

        double calculateAngle( double acceleration1, double acceleration2 );
        double calculateCalibratedValue( QString axis, int time );
        int getTime(int i);

        void at(int i, double data[ipo::SIZE]) const;
        int from, to;

        void clearSamples();
        void saveSamples(const QString& filename) const;
        void loadSamples(const QString& filename);
        void edit(const QString& option);

signals:

	void sampleAdded();
	void samplesCleared();

public slots:
	void onReceiveMessage(const ActiveMessage & msg); 


private:
        void csvToSample(const QString& line); // FIXME

	QVarLengthArray<Sample> samples;
        QVarLengthArray<Sample> copySamples;
        double accelCalibrationData[12];
        double gyroCalibrationData[12];
        double gyroMinAvgs[3];
        int accelIdleWindowStart[6];
        int gyroIdleWindowStart[7]; // FIXME

        Application &application; // FIXME

};

#endif // DATARECORDER_H
