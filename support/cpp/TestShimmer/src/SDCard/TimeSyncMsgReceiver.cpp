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

#include <ctime>
#include <QDebug>
#include <QRunnable>
#include <QThreadPool>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include "ActiveMessage.hpp"
#include "TimeSyncMsgReceiver.hpp"
#include "SerialListener.h"

using namespace std;
using namespace sdc;

typedef set<VirtualMoteID> Set;
typedef pair<Set::iterator, bool > Pair;
typedef unsigned int uint;

class InsertTask : public QRunnable {

public:

    InsertTask(QMutex* mutex, Set& moteids, const VirtualMoteID& id, uint time);

    virtual void run();

private:

    void dumpToFile(const string& date);

    QMutex* setLock;

    Set& motes;

    const VirtualMoteID vmote_id;

    const uint mote_time;
};

InsertTask::InsertTask(QMutex* mutex, Set& moteids, const VirtualMoteID& id, uint time)
    : setLock(mutex), motes(moteids), vmote_id(id), mote_time(time)
{

}

void InsertTask::run() {

    QMutexLocker lock(setLock);

    Pair result = motes.insert(vmote_id);

    if (result.second==true) {

        time_t booted = time(NULL);

        uint correction = mote_time/1024; // TODO Magic number used...

        booted -= (correction);

        //qDebug() << "Booted: " << ctime(&booted);
        //qDebug() << "Correction: " << correction << " s";

        dumpToFile(ctime(&booted));
    }
}

void InsertTask::dumpToFile(const string& date) {

    extern const QString* rootDirPath;

    QString db_filename(*rootDirPath);

    db_filename.append("rec/motes.ddb"); // FIXME Move to constants?

    QFile file(db_filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        //qDebug() << "Failed to open file: " << name;
        return;
    }

    QTextStream out(&file);

    out << vmote_id.mote_id() << '\t' << vmote_id.first_block() << '\t';
    out << mote_time << '\t' << date.c_str();

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

    VirtualMoteID vmote_id(msg.source, msg.getUInt(0));

    uint time = msg.getUInt(4);

    QRunnable* insertTask = new InsertTask(&setLock, motes, vmote_id, time);

    QThreadPool::globalInstance()->start(insertTask);
}
