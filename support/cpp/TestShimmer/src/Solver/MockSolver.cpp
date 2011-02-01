/** Copyright (c) 2011, University of Szeged
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

#include <stdexcept>
#include <QDateTime>
#include <QDebug>
#include <QRunnable>
#include <QString>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>
#include "MockSolver.hpp"

using std::logic_error;

namespace {

    const bool SUCCESS(false);
    const bool FAILED(true);
}

namespace ipo {

void SleepTask::run() {

    QMutex mutex;
    mutex.lock();

    QWaitCondition waitCondition;
    waitCondition.wait(&mutex, 2000);

    emit done();
}

MockSolver::MockSolver() : mutex(new QMutex) {

}

bool MockSolver::start(const double begin, const double end, const double length) {

    if (!mutex->tryLock()) {

        throw logic_error("already running");
    }

    qDebug() << "MockSolver::start() begin: " << begin << ", end: " << end << "length: " << length;

    if (!(0 <= begin && begin < end && end <= length)) {

        throw logic_error("0 <= begin && begin < end && end <= length should be true");
    }

    beg = begin;

    return startSolver();
}

bool MockSolver::startSolver() {

    const int sec = QDateTime::currentDateTime().time().second() % 10;

    if (0<= sec && sec <= 2) {

        qDebug() << "Emulated failure of start at " << beg;

        mutex->unlock();

        return FAILED;
    }

    SleepTask* task = new SleepTask;

    connect(task, SIGNAL(done()), SLOT(finished()));

    QThreadPool::globalInstance()->start(task);

    return SUCCESS;
}

void MockSolver::finished() {

    QString message("Emulated error ");

    bool error = SUCCESS;

    const int sec = QDateTime::currentDateTime().time().second() % 10;

    if (6<= sec && sec <= 8) {

        message.append(QString::number(beg));

        error = FAILED;
    }

    mutex->unlock();

    emit solver_done(error, message);
}

MockSolver::~MockSolver() {

    delete mutex;
}

}
