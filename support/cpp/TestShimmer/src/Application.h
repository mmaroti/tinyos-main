#ifndef APPLICATION_H
#define APPLICATION_H

#define C_HZ 204.8
#define C_TICKS 2       //constant hz and ticks values to use later

#include <QObject>
#include <QSettings>
#include "SerialListener.h"
#include "DataRecorder.h"
#include "CalibrationModule.h"
#include "ConsoleWidget.h"

class Application : public QObject
{
Q_OBJECT
public:
	Application();

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
