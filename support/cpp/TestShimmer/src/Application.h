#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QSettings>
#include "SerialListener.h"
#include "DataRecorder.h"
#include "CalibrationModule.h"

class Application : public QObject
{
Q_OBJECT
public:
	Application();

signals:
	void showMessageSignal(const QString & msg);

public:
	void showMessage(const QString & msg) {
		emit showMessageSignal(msg);
	}

public:
	SerialListener serialListener;
	DataRecorder dataRecorder;
	QSettings settings;
};

#endif // APPLICATION_H
