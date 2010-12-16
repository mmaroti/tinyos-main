/* Copyright (c) 2010, University of Szeged
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
*      Author: Ali Baharev
*/

// FIXME Remove iostream when ready
#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include "TimeSyncMsgReceiver.hpp"
#include "SerialListener.h"
#include "Utility.hpp"

using namespace std;
using namespace sdc;

typedef map<VirtualMoteID, string> Map;
typedef pair<Map::iterator, bool > Pair;

class InsertTask : public QRunnable {

public:

    InsertTask(QMutex* mutex, Map& motemap, const VirtualMoteID& id);

    virtual void run();

private:

    void dumpToFile(const string& date);

    QMutex* mapLock;

    Map& motes;

    const VirtualMoteID vmote_id;
};

InsertTask::InsertTask(QMutex* mutex, Map& motemap, const VirtualMoteID& id)
    : mapLock(mutex), motes(motemap), vmote_id(id)
{

}

void InsertTask::run() {

    QMutexLocker lock(mapLock);

    string date = current_time();

    Pair result = motes.insert(make_pair(vmote_id, date));

    if (result.second==true) {

        dumpToFile(date);
    }
}

void InsertTask::dumpToFile(const string& date) {

    extern const QString* rootDirPath;

    QString name(*rootDirPath);

    name.append("rec/motes.ddb"); // FIXME Move to constants?

    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        //qDebug() << "Failed to open file: " << name;
        return;
    }

    QTextStream out(&file);

    out << vmote_id.mote_id() << '\t' << vmote_id.first_block() << '\t' << date.c_str();

    out.flush();

    file.close();

    //qDebug() << "Data written to " << name;
}

const int TIME_SYNC_MSG = 0x3D;

void TimeSyncMsgReceiver::onReceiveMessage(const ActiveMessage& msg) {

    if (msg.type != TIME_SYNC_MSG) {

        return;
    }

    //cout << "mote " << msg.source << ", block " << msg.getInt(0) << ", time " << msg.getInt(4) << endl;

    VirtualMoteID vmote_id(msg.source, msg.getInt(0));

    QRunnable* insertTask = new InsertTask(&mapLock, motes, vmote_id);

    QThreadPool::globalInstance()->start(insertTask);
}
