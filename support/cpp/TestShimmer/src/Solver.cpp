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
#include <stdexcept>
#include "QMessageBox"
#include "QMutex"
#include "QSettings"
#include "Solver.hpp"
#include "DataReader.hpp"
#include "DataReadException.hpp"
#include "DataWriter.hpp"
#include "DataWriteException.hpp"
#include "Results.hpp"

using namespace std;

namespace {

// TODO Make an enum with ERROR_INTERNAL ?
const bool FAILED(true);
const bool SUCCESS(false);

}

namespace ipo {

void Solver::cleanup_solver() {

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

}

void Solver::cleanup_data() {

    delete r;
    r = 0;

    msg = "Error: ";
}

void Solver::cleanup_all() {

    cleanup_solver();

    cleanup_data();
}

void Solver::init() {

    cleanup_all();

    r = new Results();

    solver = new QProcess(this);

    QObject::connect(solver, SIGNAL(started()),
                       this, SLOT(  started()), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(error(QProcess::ProcessError)),
                       this, SLOT(  error(QProcess::ProcessError)), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(finished(int, QProcess::ExitStatus)),
                       this, SLOT(  finished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
}

Solver::Solver() : mutex(new QMutex), solver(0), msg(""), r(0) {

    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
}

// Entry point
bool Solver::start() {

    bool result = SUCCESS;

    // Released by emit_signal() and below if no samples are loaded
    string mboxText("Error: ");
    if (!mutex->tryLock()) {
        result = FAILED;
        mboxText += "the solver is already running!";
    }
    else if (n_samples() < 1) {
        mutex->unlock();
        result = FAILED;
        mboxText += "perhaps no samples are loaded?";
    }

    if (result==FAILED) {
        QMessageBox mbox;
        mbox.setText(mboxText.c_str());
        mbox.exec();
        return FAILED;
    }

    init();

    solver->start("gyro.exe");

    cout << endl << "External gyro.exe called" << endl;

    return result;
}

void Solver::emit_signal(bool error) {

    cout << endl << "Emitting signal: " << (error?"FAILED":"SUCCESS") << ", message: " << msg << endl;

    cleanup_solver();

    emit solver_done(error, msg.c_str(), r);

    cout << "Releasing resources NOW!" << endl;

    cleanup_data();

    mutex->unlock();
}

void Solver::started() {

    try {

        DataWriter dw(solver);

        dw.writeAll();

        cout << endl << "Input data written to gyro.exe" << endl;
    }
    catch (DataWriteException& ) {

        // FIXME Is write error also signalled?

        msg += "failed to pass input data to the solver!";

        emit_signal(FAILED);
    }
}

void Solver::error(QProcess::ProcessError error) {

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

    emit_signal(FAILED);
}

bool Solver::read_results() {

    bool result = SUCCESS;

    QList<QByteArray> arr( solver->readAll().split('\n') );

    try {

        DataReader dr(arr.begin(), arr.end());

        dr.readAll(*r);

        msg = "Successfully finished!";
    }
    catch (DataReadException& e) {

        msg += e.what();
        msg += "!";
        result = FAILED;
    }

    return result;
}

bool Solver::process_result(int exitCode) {

    using namespace gyro;

    bool result = FAILED;

    if (exitCode == gyro::SUCCESS) {

        result = read_results();
    }
    else if (exitCode == ERROR_READING_INPUT) {
        msg += "reading input!";
    }
    else if (exitCode == ERROR_INITIALIZATION) {
        msg += "initializing IPOPT!";
    }
    else if (exitCode == ERROR_INVALID_OPTION) {
        msg += "invalid option set in ipopt.opt (check the log)!";
    }
    else if (exitCode == ERROR_OPTIMIZATION) {
        msg += "optimization failed (check the log)!";
    }
    else if (exitCode == ERROR_UNKNOWN) {
        msg += "unkown error, most likely an unexpected exception in the solver!";
    }
    else if (exitCode == ERROR_READING_CONFIG) {
        msg += "the solver failed to read the configuration file!";
    }
    else {
        throw logic_error("Unknown value returned by the solver, implementation not updated properly!");
    }

    return result;
}

void Solver::finished(int exitCode, QProcess::ExitStatus exitStatus) {

    bool result = FAILED;

    if (exitStatus == QProcess::NormalExit) {
        result = process_result(exitCode);
    }
    else if (exitStatus == QProcess::CrashExit) {
        msg += "the solver crashed!";
    }
    else {
        msg += "undocumented error code received from Qt!";
    }

    emit_signal(result);
}

Solver::~Solver() {

    cleanup_all();

    delete mutex;
}

}
