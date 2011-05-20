/** Copyright (c) 2010, 2011, University of Szeged
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

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include "QObject"
#include "QProcess"

class QMutex;

namespace ipo {

class Results;

class Solver : public QObject {

    Q_OBJECT

public:

    Solver();

    // returns true if an error occured and finished won't be signalled
    bool start(const int beginSecond, const int endSecond, const int lengthSecond);

    ~Solver();

signals:

    void solver_done(bool error, const QString message);

private slots:

    void started();

    void error(QProcess::ProcessError error);

    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:

    Solver(const Solver& );
    Solver& operator=(const Solver& );

    bool get_lock();
    void unlock();
    bool set_sample_subrange(const int begin, const int end, const int length);
    bool write_solver_input();
    void start_solver();
    void init();
    void cleanup_solver();
    void cleanup_data();
    void cleanup_all();
    void emit_signal(bool error);

    bool process_result(int exitCode);

    bool write_samples();
    bool read_results();

    // FIXME Why aren't these volatile ???

    QMutex* const mutex;

    QProcess* solver;

    std::string msg;

    Results* result_data;
};

}

#endif
