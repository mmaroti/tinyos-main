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

#include <QTextStream>
#include <qextserialport.h>
#include "PortListener.h"
#include "configure.h"
PortListener::PortListener(QextSerialPort * port, QObject * parent):
	QObject(parent)
{
    this->port = port;
}

//7e 45 0 ffffffff ffffffff 0 1 2 0 6 1 1c ffffffa6 ffffff86 7e
void PortListener::receive()
{
    char data[1024];
    int bytesRead = port->read(data, 1024);
    data[bytesRead] = '\0';

    for(int i=0;i<bytesRead;i++) {
        if (data[i] == SYNC_BYTE && dataBuffer.size()) {
            dataBuffer.append(data[i]);
            process();
        } else {
            dataBuffer.append(data[i]);
        }
    }
}

void PortListener::process() {
    int data = 0;
    if (MOTE_DEBUG) fprintf(stderr, "\n#%2d#",dataBuffer.size());
    if (dataBuffer.size() == PKGLEN) {
        int int1 = dataBuffer[10];
        int int2 = dataBuffer[11];
        if (int1 < 0) int1 += 256;
        if (int2 < 0) int2 += 256;
        data = (int1 << 8) + int2;
        if (MOTE_DEBUG) fprintf(stderr, "{[%d] %4d, %4d}\n", data, int1, int2);
    }
    dataBuffer.clear();
    emit incomingData(sin(double(data)*M_PI/24));
}

void PortListener::reportClose()
{
	QTextStream out(stdout);
	
	out << "closing port" << endl;	
}

void PortListener::reportDsr(bool status)
{
	QTextStream out(stdout);
	
	if (status)
		out << "device was turned on" << endl;
	else
		out << "device was turned off" << endl;
}
