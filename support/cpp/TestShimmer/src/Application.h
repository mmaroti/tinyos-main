#ifndef APPLICATION_H
#define APPLICATION_H

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
        void showConsolSignal(const QString & msg);

public:
	void showMessage(const QString & msg) {
		emit showMessageSignal(msg);
	}

        void showConsolMessage(const QString & msg) {
                emit showConsolSignal(msg);
        }

public:
	SerialListener serialListener;
	DataRecorder dataRecorder;

	QSettings settings;
};

#endif // APPLICATION_H
