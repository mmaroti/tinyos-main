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
#include <QMessageBox>
#include <QMutex>
#include <QMetaType>
#include "Solver.hpp"
#include "Data.hpp"
#include "DataReader.hpp"
#include "DataWriter.hpp"
#include "Range.hpp"
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

        solver->disconnect();
        solver->close();
        delete solver;
        solver = 0;
    }

}

void Solver::cleanup_data() {

    delete result_data;
    result_data = 0;

    msg = "Error: ";
}

void Solver::cleanup_all() {

    cleanup_solver();

    cleanup_data();
}

void Solver::init() {

    cleanup_all();

    result_data = new Results();

    solver = new QProcess(this);

    connect(solver, SIGNAL(started()), SLOT(started()) );

    connect(solver, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)) );

    connect(solver, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)) );

}

Solver::Solver() : mutex(new QMutex), solver(0), msg(""), result_data(0) {

    qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
}

bool Solver::write_samples() {

    try {

        DataWriter dw;

        dw.writeAll(gyro::SAMPLE_FILE);

        cout << endl << "Samples are successfully written" << endl;

    }
    catch (ios_base::failure& ) {

        msg = "(ios_base::failure)";

        return FAILED;
    }
    catch (exception& e) {

        msg = string("(") + e.what() + string(")");

        return FAILED;
    }

    return SUCCESS;
}

void displayErrorMsg(const QString& what) {

    QString mboxText("Error: ");
    mboxText.append(what);

    QMessageBox mbox;
    mbox.setText(mboxText);
    mbox.exec();
}

bool Solver::get_lock() {

    if (!mutex->tryLock()) {

        displayErrorMsg("the solver is already running!");

        return FAILED;
    }

    return SUCCESS;
}

bool Solver::set_sample_subrange(const int begin, const int end, const int length) {

    try {

        if (begin==FROM_BEGINNING && end==TO_END && length==WHOLE_REC) {

            mark_all();
        }
        else {

            set_markers_second(begin, end, length);
        }
    }
    catch (logic_error& e) {

        string msg("unexpected error ");

        msg += e.what();

        displayErrorMsg(msg.c_str());

        return FAILED;
    }

    return SUCCESS;
}

bool Solver::write_solver_input() {

    if (write_samples() == FAILED) {

        QString message("failed to pass input data to the solver! ");

        message.append(msg.c_str());

        displayErrorMsg(message);

        return FAILED;
    }

    return SUCCESS;
}

void Solver::unlock() {

    clear_markers();

    msg = "Error: ";

    mutex->unlock();
}

void Solver::start_solver() {

    init();

    QStringList args;
    args.append(gyro::SAMPLE_FILE);
    args.append(gyro::RESULT_FILE);

    solver->start(gyro::SOLVER_EXE, args);

    cout << endl << "External gyro.exe called" << endl;
}

// Entry point
// Lock is released by:
// - emit_signal()
// - if setting subrange fails (logic_error)
// - if writing samples fails
bool Solver::start(const int begin, const int end, const int length) {

    if (get_lock() == FAILED) {

        return FAILED;
    }

    if (set_sample_subrange(begin, end, length) == FAILED) {

        unlock();

        return FAILED;
    }

    if (write_solver_input() == FAILED) {

        unlock();

        return FAILED;
    }

    start_solver();

    return SUCCESS;
}

void Solver::emit_signal(bool error) {

    cout << endl << "Emitting signal: " << (error?"FAILED":"SUCCESS") << ", message: " << msg << endl;

    cout << "Releasing resources NOW!" << endl;

    const QString message(msg.c_str());

    cleanup_all();

    unlock();

    emit solver_done(error, message);
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
        msg += "unexpected error code received from Qt!";
    }

    emit_signal(FAILED);
}

bool Solver::read_results() {

    try {

        DataReader dr;

        dr.readAll(gyro::RESULT_FILE, *result_data);

        load(*result_data);

        msg = "Successfully finished!";
    }
    catch (ios_base::failure& ) {

        msg += "failed to read the output of the solver!";
        return FAILED;
    }
    catch (exception& e) {

        msg += "unexpected error ";
        msg += e.what();
        return FAILED;
    }

    return SUCCESS;
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
        msg += "unknown value returned by the solver, implementation not updated properly!";
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

        msg += "unexpected error code received from Qt!";
    }

    emit_signal(result);
}

Solver::~Solver() {

    cleanup_all();

    delete mutex;
}

}
