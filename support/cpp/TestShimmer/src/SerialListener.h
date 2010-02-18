#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H

#include <QObject>
#include <QString>

class ActiveMessage
{
public:
	int dest;
	int source;
	int group;
	int type;

	QByteArray payload;

public:
	unsigned char getByte(int index) const;
	unsigned short getShort(int index) const;
	unsigned int getInt(int index) const;

	QString toString() const;
};

class QextSerialPort;

class SerialListener :
	public QObject
{
	Q_OBJECT

public:
	SerialListener();
	virtual ~SerialListener();

signals:
	void receiveMessage(const ActiveMessage & msg);
	void showNotification(const QString & message, int timeout = 0);

public slots:
	void onPortChanged(const QString & portName, int baudRate);

private slots:
	void onReadyRead();
	virtual void timerEvent(QTimerEvent *event);

private:
	QextSerialPort *port;
	void disconnectPort(const char * message);

	bool escaped;
	QByteArray partialPacket;

	int badPacketCount;
	void receiveRawPacket(const QByteArray & packet);
	void receiveTosPacket(const QByteArray & packet);

	int timerId;
	int moteTime;
};

#endif // SERIALLISTENER_H
