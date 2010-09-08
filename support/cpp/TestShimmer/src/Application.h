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

#ifndef APPLICATION_H
#define APPLICATION_H

#define C_HZ 204.8
#define C_TICKS 2       //constant hz and ticks values to use later

#include <QObject>
#include <QSettings>
#include "Solver.hpp"
#include "SerialListener.h"
#include "DataRecorder.h"
//#include "StationaryCalibrationModule.h"
#include "ConsoleWidget.h"

class Application : public QObject
{
Q_OBJECT

private:
    Solver solver;


public:
	Application();

        void startSolver();

signals:
	void showMessageSignal(const QString & msg);
        void showConsoleSignal(const QString & msg);

public:
	void showMessage(const QString & msg) {
		emit showMessageSignal(msg);
	}

        void showConsoleMessage(const QString & msg) {
                emit showConsoleSignal(msg);
        }

public:
	SerialListener serialListener;
        DataRecorder dataRecorder;

	QSettings settings;
};

#endif // APPLICATION_H
