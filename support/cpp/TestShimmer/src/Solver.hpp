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
#include "Data.hpp"

class QMutex;

namespace ipo {

class Solver : public QObject {

    Q_OBJECT

public:

    Solver();

    void start();

    double R(int sample, int i, int j) const;

    ~Solver();

signals:

    void finished(bool error, const std::string& msg);

private slots:

    void started();

    void error(QProcess::ProcessError error);

    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:

    Solver(const Solver& );
    Solver& operator=(const Solver& );

    void init();
    void destroy();
    void emit_signal(bool error, const std::string& msg);
    bool write_n_samples();
    bool write_samples();
    bool write_data(double data[SIZE]);
    bool process_result(int exitCode, std::string& msg);
    bool copy_rotation_matrices(std::string& msg);

    QMutex* const mutex;

    QProcess* solver;

    int n;

    double* m;

};

}

#endif // SOLVER_HPP
