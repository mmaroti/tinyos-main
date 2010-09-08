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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "QMutex"
#include "QSettings"
#include "Solver.hpp"
#include "ErrorCodes.hpp"

using namespace std;

namespace {

    const bool FAILED(true);
    const bool SUCCESS(false);
}

void Solver::destroy() {

    if (solver!=0) {
        // FIXME What if still running?

        QObject::disconnect(solver, SIGNAL(started()),
                              this, SLOT(  started()));

        QObject::disconnect(solver, SIGNAL(error(QProcess::ProcessError)),
                              this, SLOT(  error(QProcess::ProcessError)));

        QObject::disconnect(solver, SIGNAL(finished(int, QProcess::ExitStatus)),
                              this, SLOT(  finished(int, QProcess::ExitStatus)));

        solver->close();
        delete solver;
        solver = 0;
    }

    n = 0;
    delete[] m;
    m = 0;
}

void Solver::init() {

    destroy();

    solver = new QProcess(this);

    QObject::connect(solver, SIGNAL(started()),
                       this, SLOT(  started()), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(error(QProcess::ProcessError)),
                       this, SLOT(  error(QProcess::ProcessError)), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(finished(int, QProcess::ExitStatus)),
                       this, SLOT(  finished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
}

Solver::Solver() : mutex(new QMutex), solver(0), n(0), m(0) {

    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
}

// Entry point
void Solver::start() {

    // Released by emit_signal()
    if (!mutex->tryLock()) {

        throw logic_error("The solver is already running!");
    }

    init();

    solver->start("gyro.exe");

    cout << endl << "External gyro.exe called" << endl;
}

void Solver::emit_signal(bool error, const std::string &msg) {

    emit finished(error, msg);

    mutex->unlock();
}

bool Solver::write_data(double data[SIZE]) {

    bool result = SUCCESS;

    for (int i=0; i<SIZE; ++i) {
        ostringstream os;
        os << setprecision(16) << scientific;
        os << data[i] << '\n' << flush;
        int k = solver->write(os.str().c_str());
        if (k == -1) {
            result = FAILED;
            break;
        }
    }

    return result;
}

bool Solver::write_samples() {

    bool result = SUCCESS;

    double data[SIZE];

    const int n = n_samples();

    for (int i=0; i<n; ++i) {

        at(i, data);

        result = write_data(data);

        if (result == FAILED) {
            break;
        }
    }

    return result;
}

bool Solver::write_n_samples() {

    const int n = n_samples();

    if (n<1)
        throw logic_error("Incorrect number of samples!");

    ostringstream os;

    os << n << '\n' << flush;

    int k = solver->write(os.str().c_str());

    return (k==-1)?FAILED:SUCCESS;
}

void Solver::started() {

    bool result = write_n_samples();

    if (result==SUCCESS) {

        result = write_samples();
    }

    if (result == FAILED) {

        emit_signal(FAILED, "Error on passing data to the solver!");
    }

    cout << endl << "Input data written to gyro.exe" << endl;
}

void Solver::error(QProcess::ProcessError error) {

    string msg("Error: ");

    if (error == QProcess::FailedToStart) {
        msg += "the solver failed to start!";
    }
    else if (error == QProcess::Crashed) {
        msg += "the solver crashed!";
    }
    else if (error == QProcess::Timedout) {
        msg += "the solver timed out!";
    }
    else if (error == QProcess::ReadError) {
        msg += "when retrieving data from the solver!";
    }
    else if (error == QProcess::WriteError) {
        msg += "when passing data to the solver!";
    }
    else if (error == QProcess::UnknownError) {
        msg += "unknown error!";
    }
    else {
        msg += "undocumented error code received from Qt!";
    }

    emit_signal(FAILED, msg);
}

bool Solver::copy_rotation_matrices(string& msg) {

    if ((n!=0) || (m!=0))
        throw logic_error("The rotation matrices should have been released!");

    QList<QByteArray> arr( solver->readAll().split('\n') );

    const int size = arr.size();
    n = n_samples();

    int n_elem = 9*n;
    // It seems it appends additional newlines, perhaps EOF?
    if (size < n_elem+1)
        throw runtime_error("Unexpected output from the solver!");

    m = new double[n_elem];

    QList<QByteArray>::iterator i = arr.begin();
    // TODO Process the first line!
    ++i;

    QList<QByteArray>::iterator end = arr.end();

    bool ok = false;

    for (int k=0; k<n_elem; ++i, ++k) {

        if (i==end)
            throw logic_error("Bug in: " __FILE__ );

        m[k] = i->toDouble(&ok);

        if (!ok)
            break;
    }

    bool result = FAILED;

    if (ok) {
        result = SUCCESS;
        msg = "Finished";
    }
    else {
        result = FAILED;
        msg += "unexpected error when processing the output of the solver!";
    }

    return result;
}

bool Solver::process_result(int exitCode, string& msg) {

    bool result = FAILED;

    if (exitCode == gyro::SUCCESS) {

        result = copy_rotation_matrices(msg);
    }
    else if (exitCode == gyro::ERROR_READING_INPUT) {
        msg += "reading input!";
    }
    else if (exitCode == gyro::ERROR_INITIALIZATION) {
        msg += "initializing IPOPT!";
    }
    else if (exitCode == gyro::ERROR_CONVERGENCE) {
        msg += "regression failed to converge!";
    }
    else if (exitCode == gyro::ERROR_UNKNOWN) {
        msg += "unkown error, most likely an unexpected exception in the solver!";
    }
    else {
        throw logic_error("Unknown value returned by the solver, implementation not updated properly!");
    }

    return result;
}

void Solver::finished(int exitCode, QProcess::ExitStatus exitStatus) {

    bool result = FAILED;
    string msg("Error: ");

    if (exitStatus == QProcess::NormalExit) {
        result = process_result(exitCode, msg);
    }
    else if (exitStatus == QProcess::CrashExit) {
        msg += "the solver crashed!";
    }
    else {
        msg += "undocumented error code received from Qt!";
    }

    emit_signal(result, msg);
}

double Solver::R(int sample, int i, int j) const {

    return 0;
}

Solver::~Solver() {

    destroy();

    delete mutex;
}
