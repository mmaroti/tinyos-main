#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include "SerialListener.h"
#include "DataRecorder.h"

class Application : public QObject
{
Q_OBJECT
public:
	Application();

public:
	SerialListener serialListener;
	DataRecorder dataRecorder;
};

#endif // APPLICATION_H
