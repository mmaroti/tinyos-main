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
#include "SerialListener.h"

#ifndef DATARECORDER_H
#define DATARECORDER_H

#define MAX_MOTE 5

class Application;

enum Coordinate { X, Y, Z };

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
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
        DataRecorder(Application &app);
        ~DataRecorder();

	int size() const {
		return samples.size();
	}

	const Sample & at(int i) const {
		return samples[i];
	}

        double getLastXAccel(){
            return (double)samples[samples.size()-1].xAccel;
        }

        bool empty() const {
            return samples.isEmpty();
        }

signals:

        void sampleAdded();

public slots:
        void onReceiveMessage(const ActiveMessage & msg);

private:

	QVarLengthArray<Sample> samples;

        Application &application; // FIXME

};

#endif // DATARECORDER_H
