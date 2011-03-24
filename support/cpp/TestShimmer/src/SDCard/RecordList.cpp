/* Copyright (c) 2010, 2011 University of Szeged
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
#include "TimeSyncCalc.hpp"
#include "TimeSyncData.hpp"
#include "TimeSyncMerger.hpp"

RecordList::RecordList() :
        mutex(new QMutex),
        header(new QVector<MoteHeader>),
        records(new QVector<RecordLine>),
        matching_header(new QVector<MoteHeader>),
        matching_records(new QVector<RecordLine>),
        matching_timesync_data(new QVector<TimeSyncData>),
        scout(new sdc::RecordScout)
{

}

RecordList::~RecordList() {

    delete mutex;
    delete header;
    delete records;
    delete matching_header;
    delete matching_records;
    delete matching_timesync_data;
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

    //qDebug() << c_std_date;

    const QDateTime date = QDateTime::fromString(c_std_date);

    //qDebug() << date.addDays(10).toString();

    return date;
}

const MoteHeader MoteInfo2MoteHeader(const sdc::MoteInfo& m) {

    QDateTime last_download = stdDate2QDate(m.last_download().c_str());

    return MoteHeader(m.mote_id(), last_download, m.remaining_hours().c_str(), m.number_of_records());
}

// TODO Try to eliminate duplication
void RecordList::copy_headers() {

    const std::vector<sdc::MoteInfo>& moteinfo = scout->headers();

    header->clear();

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

// TODO Try to eliminate duplication
void RecordList::copy_lines() {

    const std::vector<sdc::RecordInfo>& recordinfo = scout->record_info();

    records->clear();

    const int n = static_cast<int> (recordinfo.size());

    records->reserve(n);

    for (int i=0; i<n; ++i) {

        records->append(RecordInfo2RecordLine(recordinfo.at(i)));
    }
}

void RecordList::read_all() {

    matching_header->clear(); // TODO Check what is good for the GUI
    matching_records->clear();
    matching_timesync_data->clear();

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

// FIXME How can this duplication be eliminated?
void RecordList::search_for_matching_records(int mote, int reboot) {

    qDebug() << "Entered RecordList::search_for_matching_records(" << mote << ", " << reboot << ")";

    if (!mutex->tryLock()) {

        show_lock_error();

        return;
    }

    try {

        search_for_matching(mote, reboot);
    }
    catch (std::exception& e) {

        show_read_error(e.what(), typeid(e).name());
    }

    mutex->unlock();

    qDebug() << "Finished RecordList::search_for_matching_records()";
}

void RecordList::copy_matching_header(const sdc::RecordID& rid) {

    const sdc::MoteInfo& m = scout->find_moteinfo(rid);

    const MoteHeader moteheader = MoteInfo2MoteHeader(m);

    if (matching_header->empty()||matching_header->last().mote_id()<moteheader.mote_id()) {

        matching_header->push_back(moteheader);
    }
    else if (matching_header->last().mote_id()!=moteheader.mote_id()) {

        throw std::logic_error("mote ID already inserted");
    }
}

void RecordList::copy_matching_line(const sdc::RecordID& rid) {

    const sdc::RecordInfo& r = scout->find_recordinfo(rid);

    matching_records->push_back(RecordInfo2RecordLine(r));
}

void RecordList::search_for_matching(int mote, int reboot) {

    matching_header->clear();
    matching_records->clear();
    matching_timesync_data->clear();

    sdc::TimeSyncMerger merger(mote, reboot);

    typedef std::set<sdc::RecordID> Set;

    const Set& matching = merger.recordID_of_pairs();

    int size = 0;

    for (Set::const_iterator i=matching.begin(); i!=matching.end(); ++i) {

        copy_matching_header(*i);
        copy_matching_line(*i);

        ++size;
    }

    matching_timesync_data->resize(size);

    sdc::TimeSyncCalc fill_timesync_data(merger, matching_timesync_data->data(), size);

    qDebug() << "Set size:     " << matching.size();
    dump_matching_data();
}

void RecordList::dump_matching_data() const {

    qDebug() << "Headers";

    for (int i=0; i<matching_header->size(); ++i) {

        const MoteHeader& m = matching_header->at(i);

        qDebug() << m.mote_id();
    }

    qDebug() << "Records";

    for (int i=0; i<matching_records->size(); ++i) {

        const RecordLine& r = matching_records->at(i);

        qDebug() << r.mote_id() << "  " << r.record_id();
    }
}

const QVector<MoteHeader>& RecordList::matching_headers() const {

    check_lock();

    return *matching_header;
}

const QVector<RecordLine>& RecordList::matching_record_info() const {

    check_lock();

    return *matching_records;
}
