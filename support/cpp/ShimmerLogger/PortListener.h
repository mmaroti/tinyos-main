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
* - Neither the name of the University of Szeged nor the names of its
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
*/

/**
* @author Andras Becsi (abecsi@inf.u-szeged.hu)
*/

#ifndef PORTLISTENER_H
#define PORTLISTENER_H

#include <QObject>

class QextSerialPort;


static const char SYNC_BYTE = 0x7e;
static const char ESCAPE_BYTE = 0x7d;
static const char ESCAPE_XOR_BYTE = 0x20;
static const int PKGLEN = 15;

/**
 * Port Listener.
 */
class PortListener : public QObject
{
    Q_OBJECT

    QextSerialPort * port;
    QList<char> dataBuffer;
    void process();
public:
    PortListener(QextSerialPort * port, QObject * parent = 0);

	public slots:
    /**
		 * Receive data from serial port.
		 */
    void receive();
    /**
		 * Report port closing.
		 */
    void reportClose();

    /**
		 * Report DSR line.
		 * 	@param status line status.
		 */
    void reportDsr(bool status);
signals:
    void incomingData(double);

};


#endif // PORTLISTENER_H
