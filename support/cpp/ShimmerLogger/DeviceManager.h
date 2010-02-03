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

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H
#include <QObject>
#include <qextserialenumerator.h>
#include <qextserialport.h>
#include <stdio.h>

/*
   * Protocol inspired by, but not identical to, RFC 1663. There is
   * currently no protocol establishment phase, and a single byte
   * ("packet type") to identify the kind/target/etc of each packet.
   *
   * The protocol is really, really not aiming for high performance.
   *
   * There is however a hook for future extensions: implementations
   * are required to answer all unknown packet types with a P_UNKNOWN
   * packet.
   *
   * To summarise the protocol:
   * - the two sides (A & B) are connected by a (potentially
   *   unreliable) byte stream
   *
   * - the two sides exchange packets framed by 0x7e (SYNC_BYTE) bytes
   *
   * - each packet has the form
   *     <packet type> <data bytes 1..n> <16-bit crc>
   *   where the crc (see net.tinyos.util.Crc) covers the packet type
   *   and bytes 1..n
   *
   * - bytes can be escaped by preceding them with 0x7d and their
   *   value xored with 0x20; 0x7d and 0x7e bytes must be escaped,
   *   0x00 - 0x1f and 0x80-0x9f may be optionally escaped
   *
   * - There are currently 5 packet types:
   *   P_PACKET_NO_ACK: A user-packet, with no ack required
   *   P_PACKET_ACK: A user-packet with a prefix byte, ack
   *   required. The receiver must send a P_ACK packet with the
   *   prefix byte as its contents.
   *   P_ACK: ack for a previous P_PACKET_ACK packet
   *   P_UNKNOWN: unknown packet type received. On reception of an
   *   unknown packet type, the receicer must send a P_UNKNOWN packet,
   *   the first byte must be the unknown packet type.
   *
   * - Packets that are greater than a (private) MTU are silently
   *   dropped.
   */

#include "configure.h"
#include "PortListener.h"
#include "PlotWidget.h"
#include <QTimer>
#include <QThread>

class DeviceManager : public QThread {
    Q_OBJECT
private:
    QextSerialPort* device;
    PortListener* listener;
    PlotWidget* plotter;
    QList<double> dataExchangeBuffer;
    QTimer sendTimer;
protected:
    virtual void run();
private slots:
    void collectData(double);
public slots:
    void stop();
    void resume();
public:
    DeviceManager(QObject* parent);
    void setPlotter(PlotWidget*);
    void searchForDevices();
signals:
    void outgoingData(const QList<double>&);
};


#endif // DEVICEMANAGER_H
