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

#include <stdexcept>
#include <typeinfo>
#include <vector>
#include <QDebug>
#include <QMessageBox>
#include <QMutexLocker>
#include <QVector>
#include "RecordList.hpp"
#include "MoteHeader.hpp"
#include "RecordLine.hpp"
#include "RecordScout.hpp"

RecordList::RecordList() :
        mutex(new QMutex),
        header(new QVector<MoteHeader>),
        records(new QVector<RecordLine>),
        scout(new sdc::RecordScout)
{

}

RecordList::~RecordList() {

    delete mutex;
    delete header;
    delete records;
    delete scout;
}

void RecordList::read_all_existing() {

    qDebug() << "Entered RecordList::read_all_existing()";

    if (!mutex->tryLock()) {

        show_lock_error();

        return;
    }

    try {

        read_all();
    }
    catch (std::exception& e) {

        show_read_error(e.what(), typeid(e).name());
    }

    mutex->unlock();

    qDebug() << "Finished RecordList::read_all_existing()";
}

const QDateTime stdDate2QDate(const char* c_std_date) {

    const QDateTime date = QDateTime::fromString(c_std_date, "ddd MMM dd HH:mm:ss yyyy");

    //qDebug() << date.addDays(10).toString();

    return date;
}

const MoteHeader MoteInfo2MoteHeader(const sdc::MoteInfo& m) {

    QDateTime last_download = stdDate2QDate(m.last_download().c_str());

    return MoteHeader(m.mote_id(), last_download, m.remaining_hours().c_str(), m.number_of_records());
}

// TODO Make it a template to eliminate duplication
void RecordList::copy_headers() {

    std::vector<sdc::MoteInfo> moteinfo = scout->headers();

    const int n = static_cast<int> (moteinfo.size());

    header->reserve(n);

    for (int i=0; i<n; ++i) {

        header->append(MoteInfo2MoteHeader(moteinfo.at(i)));
    }
}

const RecordLine RecordInfo2RecordLine(const sdc::RecordInfo& r) {

    QDateTime download = stdDate2QDate(r.date_downloaded().c_str());

    QDateTime recorded; // TODO Is null date good for the GUI?

    std::string date_recorded = r.date_recorded();

    if (date_recorded.size()>0) {

        recorded = stdDate2QDate(date_recorded.c_str());
    }

    return RecordLine(r.mote_id(), r.record_id(), r.length().c_str(), download, recorded);
}

// TODO Make it a template to eliminate duplication
void RecordList::copy_lines() {

    std::vector<sdc::RecordInfo> recordinfo = scout->record_info();

    const int n = static_cast<int> (recordinfo.size());

    records->reserve(n);

    for (int i=0; i<n; ++i) {

        records->append(RecordInfo2RecordLine(recordinfo.at(i)));
    }
}

void RecordList::read_all() {

    header->clear();
    records->clear();

    scout->read_all_existing();

    //scout->dump_all();

    qDebug() << "Number of records: " << scout->record_info().size();

    copy_headers();
    copy_lines();
}

void RecordList::show_read_error(const char* what, const char* name) const {

    QString message(what);

    message.append(" (");
    message.append(name);
    message.append(")");

    show_error(message);
}

// Makes sense only when debugging, NOT for thread synchronization
void RecordList::check_lock() const {

    if (!mutex->tryLock()) {

        show_lock_error();
    }
    else {

        mutex->unlock();
    }
}

void RecordList::show_lock_error() const {

    show_error("member function called when still looking for records");
}

void RecordList::show_error(const QString &msg) const {

    QString message("Error: ");
    message.append(msg);
    message.append("!");

    QMessageBox mbox;
    mbox.setText(message);
    mbox.exec();
}

const QVector<MoteHeader>& RecordList::headers() const {

    check_lock();

    return *header;
}

const QVector<RecordLine>& RecordList::record_info() const {

    check_lock();

    return *records;
}
