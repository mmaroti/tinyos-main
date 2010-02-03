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

#include "DeviceManager.h"
#include <QIODevice>

DeviceManager::DeviceManager(QObject *parent): QThread(parent) {
#ifdef Q_OS_WIN
    device = new QextSerialPort("COM4", QextSerialPort::EventDriven);
#else
    device = new QextSerialPort("/dev/ttyUSB1",  QextSerialPort::EventDriven);
#endif
    device->setBaudRate(BAUD115200);
    device->setFlowControl(FLOW_OFF);
    device->setParity(PAR_NONE);
    device->setDataBits(DATA_8);
    device->setStopBits(STOP_2);
    device->open(QIODevice::ReadOnly);

    listener = new PortListener(device, this);
}

void DeviceManager::searchForDevices() {
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    printf("List of ports:\n");
    for (int i = 0; i < ports.size(); i++) {
        printf("port name: %s\n", ports.at(i).portName.toLocal8Bit().constData());
        printf("friendly name: %s\n", ports.at(i).friendName.toLocal8Bit().constData());
        printf("physical name: %s\n", ports.at(i).physName.toLocal8Bit().constData());
        printf("enumerator name: %s\n", ports.at(i).enumName.toLocal8Bit().constData());
        printf("===================================\n\n");
    }
}

void DeviceManager::collectData(double data) {
    dataExchangeBuffer.append(data);
}

void DeviceManager::stop() {
    dataExchangeBuffer.clear();
    disconnect(listener, SIGNAL(incomingData(double)), plotter, SLOT(incomingData(double)));
    connect(listener,SIGNAL(incomingData(double)),SLOT(collectData(double)));
}

void DeviceManager::resume() {
    disconnect(listener, SIGNAL(incomingData(double)), this, SLOT(collectData(double)));
    plotter->incomingData(dataExchangeBuffer);
    connect(listener, SIGNAL(incomingData(double)), plotter, SLOT(incomingData(double)));
}

void DeviceManager::run() {
    connect(device, SIGNAL(readyRead()), listener, SLOT(receive()));
    connect(device, SIGNAL(aboutToClose()), listener, SLOT(reportClose()));
    connect(device, SIGNAL(dsrChanged(bool)), listener, SLOT(reportDsr(bool)));
}

void DeviceManager::setPlotter(PlotWidget* p) {
    plotter = p;
}
