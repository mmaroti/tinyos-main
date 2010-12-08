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
* Author: Ali Baharev
*/

#include <exception>
#include <typeinfo>
#include <QDebug>
#include <QString>
#include "DownloadTask.hpp"
#include "SDCardCreator.hpp"
#include "SDCard.hpp"

//-----------------------------------------------------------------------------
// FIXME Remove these when the implementation is ready
#include "SDataWidget.h"
#include <QWaitCondition>
#include <QMutex>

class Sleep
{

public:

    static void msleep(unsigned long msecs) {
        QMutex mutex;
        mutex.lock();

        QWaitCondition waitCondition;
        waitCondition.wait(&mutex, msecs);

        mutex.unlock();
    }
};
//-----------------------------------------------------------------------------

namespace sdc {

DownloadTask::DownloadTask(const SDCardCreator* src) : source(src) {

}

DownloadTask::~DownloadTask() {
    // Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

void DownloadTask::processMeasurements() {

    //----------------------------------
    // TODO Remove when ready
    Sleep::msleep(3000);

    fillSData(data);
    //----------------------------------

    std::auto_ptr<SDCard> sdcard(source->create());

    sdcard->process_new_measurements();
}

void DownloadTask::run() {

    qDebug() << "It will take at least 3 seconds";

    bool failed = false;

    QString msg;

    try {

        processMeasurements();
    }
    catch (std::exception& e) {

        QTextStream ts(&msg, QIODevice::WriteOnly);
        ts << "Error: " << e.what() << " (" << typeid(e).name() << ")";

        failed = true;
    }

    emit downloadFinished(failed, msg, data);

    qDebug() << "Resources deleted";
}

}
