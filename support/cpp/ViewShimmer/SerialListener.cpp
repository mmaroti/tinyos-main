/** Copyright (c) 2010, 2011, University of Szeged
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

#include "SerialListener.h"
#include "ActiveMessage.hpp"
#include "qextserialport.h"
#include <QtDebug>
#include <QDateTime>

SerialListener::SerialListener()
{
	port = NULL;
	badPacketCount = 0;
	timerId = -1;
}

SerialListener::~SerialListener()
{
}

void SerialListener::disconnectPort(const char * message)
{
	if( timerId >= 0 )
	{
		killTimer(timerId);
		timerId = -1;
	}

	if( port != NULL )
	{
		disconnect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

		// delete the port from timer, otherwise it sigfaults because
		// we might be in a different thread
		timerId = startTimer(0);
	}

        emit disconnected();
        emit showNotification(message);
}

void SerialListener::onPortChanged(const QString & portName, int baudRate)
{
	if( port != NULL || timerId >= 0 )
		disconnectPort("Disconnected.");

	if( portName == "" || baudRate == 0 )
		return;

	if( portName == "simulated" )
	{
		timerId = startTimer(20);

                emit connected();
                emit showNotification("Generating simulated messages.");

		return;
	}

	port = new QextSerialPort(portName, QextSerialPort::EventDriven);

	if( baudRate == 57600 )
		port->setBaudRate(BAUD57600);
	else if( baudRate == 115200 )
		port->setBaudRate(BAUD115200);
	else if( baudRate == 230400 )
		port->setBaudRate(BAUD230400);
	else if( baudRate == 460800 )
		port->setBaudRate(BAUD460800);
	else if( baudRate == 921600 )
		port->setBaudRate(BAUD921600);

	port->setFlowControl(FLOW_OFF);
	port->setParity(PAR_NONE);
	port->setDataBits(DATA_8);
	port->setStopBits(STOP_1);

	if( port->open(QIODevice::ReadWrite) != true )
		disconnectPort("Port could not be opened.");
	else
	{
		badPacketCount = -1;
		partialPacket.clear();

                emit connected();
                emit showNotification("Connected.");

		connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	}

}

void SerialListener::onReadyRead()
{
	QByteArray bytes = port->readAll();

	for(int i = 0; i < bytes.size(); ++i)
	{
		char c = bytes.at(i);

		if( escaped )
		{
			escaped = false;
			c ^= (char)0x20;
			partialPacket.append(c);
		}
		else if( c == (char)0x7d )
			escaped = true;
		else if( c == (char)0x7e )
		{
			if( partialPacket.size() > 0 )
			{
				receiveRawPacket(partialPacket);
				partialPacket.clear();
			}
		}
		else if( partialPacket.size() > 1000 )
		{
			disconnectPort("Too long packet, disconnected.");
			return;
		}
		else
			partialPacket.append(c);
	}
}

int calcCrc(int crc, char b)
{
	crc = crc ^ ((int)b << 8);

	for(int i = 0; i < 8; i++)
	{
		if( (crc & 0x8000) == 0x8000 )
			crc = crc << 1 ^ 0x1021;
		else
			crc = crc << 1;
	}

	return crc & 0xffff;
}

void SerialListener::receiveRawPacket(const QByteArray & packet)
{
	int size = packet.size();

	if( size < 3 || packet.at(0) != (char)0x45 )
	{
		if( ++badPacketCount > 100 )
			disconnectPort("Too many bad packets, disconnected.");
		else if( badPacketCount > 0 )
                        emit showNotification("Bad packet format.");

		return;
	}

	int crc = 0;
	for(int i = 0; i < size - 2; ++i)
		crc = calcCrc(crc, packet.at(i));

	crc = calcCrc(crc, packet.at(size-1));
	crc = calcCrc(crc, packet.at(size-2));

	if( crc != 0 )
	{
		if( ++badPacketCount > 100 )
			disconnectPort("Too many bad packets, disconnected.");
		else
                        emit showNotification("Bad packet crc.");

		return;
	}

	badPacketCount = 0;

	QByteArray decoded(packet.data() + 1, size - 3);
	receiveTosPacket(decoded);
}

void SerialListener::receiveTosPacket(const QByteArray & packet)
{
	if( packet.size() >= 8 && packet.at(0) == (char)0x00 )
	{
		int length = packet.at(5) & 0xFF;

		if( packet.size() != length + 8 )
		{
                        emit showNotification("Bad packet length.");
			return;
		}

		ActiveMessage msg;

		msg.dest = ((packet.at(1) & 0xFF) << 8) + (packet.at(2) & 0xFF);
		msg.source = ((packet.at(3) & 0xFF) << 8) + (packet.at(4) & 0xFF);
		msg.group = packet.at(6) & 0xFF;
		msg.type = packet.at(7) & 0xFF;
		msg.payload.append(packet.data() + 8, length);

//              qDebug() << "msg " + msg.toString();
		emit receiveMessage(msg);
	}
}

void SerialListener::timerEvent(QTimerEvent *)
{
	if( port != NULL )
	{
		port->close();
		delete port;
		port = NULL;

		killTimer(timerId);
		timerId = -1;

		return;
	}

	ActiveMessage msg;

	msg.dest = 65535;
	msg.source = 13;
	msg.group = 0;
	msg.type = 0x37;

	for(int i = 0; i < 5; ++i)
	{
		moteTime += 160;

		msg.payload.append((char)(moteTime));
		msg.payload.append((char)(moteTime >> 8));
		msg.payload.append((char)(moteTime >> 16));
		msg.payload.append((char)(moteTime >> 24));

		for(int j = 0; j < 6; ++j)
		{
			int value = (qrand() & 0x07FF) + 0x0400;
			msg.payload.append((char)(value));
			msg.payload.append((char)(value >> 8));
		}

		int value = (moteTime >> 7) & 0x0FFF;
		msg.payload.append((char)(value));
		msg.payload.append((char)(value >> 8));

		msg.payload.append((char)0);
		msg.payload.append((char)0x09);
	}

        // Emulates message loss
        /*int sec = QDateTime::currentDateTime().time().second() % 10;
        if (4<= sec && sec <=6)
            return;*/

	emit receiveMessage(msg);
}
