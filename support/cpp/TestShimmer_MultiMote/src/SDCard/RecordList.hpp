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

#ifndef RECORDLIST_HPP
#define RECORDLIST_HPP

#include <QVector>

namespace sdc {
    class RecordID;
    class RecordScout;
}

class QMutex;
class QString;
class MoteHeader;
class RecordLine;
class TimeSyncData;

class RecordList {

public:

    RecordList();

    void read_all_existing();

    const QVector<MoteHeader>& headers() const;

    const QVector<RecordLine>& record_info() const;

    void link(int mote, int reboot);

    void search_for_matching_records(int mote, int reboot);

    const QVector<MoteHeader>& matching_headers() const;

    const QVector<RecordLine>& matching_record_info() const;

    ~RecordList();

private:

    Q_DISABLE_COPY(RecordList)

    void read_all();
    void search_for_matching(int mote, int reboot);

    void copy_headers();
    void copy_lines();

    void copy_matching_header(const sdc::RecordID& rid);
    void copy_matching_line(const sdc::RecordID& rid);
    void dump_matching_data() const;

    void show_read_error(const char* what, const char* name) const;
    void show_lock_error() const;
    void show_error(const QString& msg) const;

    void check_lock() const;

    QMutex* const mutex;
    // TODO The containers should go into their own class
    QVector<MoteHeader>* const header;
    QVector<RecordLine>* const records;
    QVector<MoteHeader>* const matching_header;
    QVector<RecordLine>* const matching_records;
    QVector<TimeSyncData>* const matching_timesync_data;
    sdc::RecordScout* const scout;

};

#endif // RECORDLIST_HPP
