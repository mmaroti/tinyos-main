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

#include <iostream>
#include <stdexcept>
#include "QMessageBox"
#include "QMutex"
#include "QSettings"
#include "Solver.hpp"
#include "Data.hpp"
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

bool Solver::write_samples() {

    bool result = SUCCESS;

    try {

        DataWriter dw;

        dw.writeAll(gyro::SAMPLE_FILE);

        cout << endl << "Samples are successfully written" << endl;

    }
    catch (DataWriteException& ) {

        result = FAILED;
    }

    return result;
}

void displayErrorMsg(const QString& what) {

    QString mboxText("Error: ");
    mboxText.append(what);

    QMessageBox mbox;
    mbox.setText(mboxText);
    mbox.exec();
}

// Released by:
// - emit_signal()
// - if writing samples fails
// FIXME Wrap up tryLock and unlock and add set/clear_markers there
bool Solver::get_lock() {

    if (!mutex->tryLock()) {
        displayErrorMsg("the solver is already running!");
        return FAILED;
    }

    mark_all();

    if (write_samples()==FAILED){
        clear_markers();
        mutex->unlock();
        displayErrorMsg("failed to pass input data to the solver!");
        return FAILED;
    }

    return SUCCESS;
}

// Entry point
bool Solver::start() {

    if (get_lock() == FAILED) {

        return FAILED;
    }

    init();

    QStringList args;
    args.append(gyro::SAMPLE_FILE);
    args.append(gyro::RESULT_FILE);

    solver->start(gyro::SOLVER_EXE, args);

    cout << endl << "External gyro.exe called" << endl;

    return SUCCESS;
}

void Solver::emit_signal(bool error) {

    cout << endl << "Emitting signal: " << (error?"FAILED":"SUCCESS") << ", message: " << msg << endl;

    cleanup_solver();

    emit solver_done(error, msg.c_str(), r);

    cout << "Releasing resources NOW!" << endl;

    cleanup_data();

    clear_markers();

    mutex->unlock();
}

void Solver::started() {

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

    try {

        DataReader dr;

        dr.readAll(gyro::RESULT_FILE, *r);

        msg = "Successfully finished!";
    }
    catch (DataReadException& ) {

        msg += "failed to read the output of the solver!";
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
    else if (exitCode == ERROR_ARG_COUNT) {
        msg += "incorrect number of arguments passed to the solver!";
    }
    else if (exitCode == ERROR_READING_INPUT) {
        msg += "reading input!";
    }
    else if (exitCode == ERROR_INCORRECT_TIMESTAMP) {
        msg += "incorrect timestamp found in the input!";
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
    else if (exitCode == ERROR_NUMBER_INVALID) {
        msg += "invalid number detected!";
    }
    else if (exitCode == ERROR_ORTHOGONALITY) {
        msg += "poor quality solution (orthogonality)!";
    }
    else if (exitCode == ERROR_OBJECTIVE) {
        msg += "poor quality solution (objective)!";
    }
    else if (exitCode == ERROR_WRITING_RESULTS) {
        msg += "the solver failed to write the results into file!";
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

        return; // FIXME Qt also signals error, it is just stupid.
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
