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
#include <cstring>
#include <stdexcept>
#include "QMessageBox"
#include "QMutex"
#include "QSettings"
#include "Solver.hpp"
#include "CompileTimeConstants.hpp"

using namespace std;

namespace {

// TODO Make an enum with ERROR_INTERNAL ?
const bool FAILED(true);
const bool SUCCESS(false);

class MutexUnlocker {

public:

    MutexUnlocker(QMutex* m) : mutex(m) {
        if (!mutex->tryLock())
            throw logic_error("The solver is still running!");
    }

    ~MutexUnlocker() { mutex->unlock(); }

private:

    MutexUnlocker(const MutexUnlocker& );
    MutexUnlocker& operator=(const MutexUnlocker& );

    QMutex* const mutex;

};

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

    n = 0;
    delete[] m;
    m = 0;
    msg = "";
}

void Solver::cleanup_all() {

    cleanup_solver();

    cleanup_data();
}

void Solver::init() {

    cleanup_all();

    solver = new QProcess(this);

    QObject::connect(solver, SIGNAL(started()),
                       this, SLOT(  started()), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(error(QProcess::ProcessError)),
                       this, SLOT(  error(QProcess::ProcessError)), Qt::QueuedConnection);

    QObject::connect(solver, SIGNAL(finished(int, QProcess::ExitStatus)),
                       this, SLOT(  finished(int, QProcess::ExitStatus)), Qt::QueuedConnection);
}

Solver::Solver() : mutex(new QMutex), solver(0), n(0), m(0), msg("") {

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

    mutex->unlock();

    emit finished(error, msg);
}

void Solver::write_data(double data[SIZE]) {

    ostringstream os;

    os << setprecision(16) << scientific;

    for (int i=0; i<SIZE; ++i) {

        os << data[i] << '\n';
    }

    os << flush;

    write(os.str().c_str());
}

void Solver::write_samples() {

    double data[SIZE];

    const int n = n_samples();

    for (int i=0; i<n; ++i) {

        at(i, data);

        write_data(data);
    }
}

void Solver::write_n_samples() {

    const int n = n_samples();

    if (n<1)
        throw logic_error("Incorrect number of samples!");

    ostringstream os;

    os << n << '\n' << flush;

    write(os.str().c_str());
}

void Solver::write_line(const char* text) {

    string line(text);

    line.push_back('\n');

    write(line.c_str());
}

void Solver::write(const char *data) {

    int k = solver->write(data);

    if (k==-1) {
        throw '\0';
    }
}

void Solver::started() {

    try {

        write_line(gyro::NUMBER_OF_SAMPLES);

        write_n_samples();

        write_line(gyro::INPUT_DATA);

        write_samples();

        cout << endl << "Input data written to gyro.exe" << endl;
    }
    catch (char ) {

        // FIXME Is write error also signalled?

        msg = "Error: failed to pass input data to the solver!";

        emit_signal(FAILED);
    }
}

void Solver::error(QProcess::ProcessError error) {

    msg = "Error: ";

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

void Solver::read_vector(const char* text, double* r, int length, li& i, cli& end) {

    skip_line(text, i, end);

    bool ok = false;

    for (int k=0; k<length; ++i, ++k) {

        if (i==end) {
            msg += "unexpected end of output!";
            throw '\0';
        }

        const char* const line = i->constData();

        r[k] = i->toDouble(&ok);

        if (!ok) {
            msg += "conversion error!";
            throw '\0';
        }
    }

}

void Solver::skip_irrelevant_lines(li& i, cli& end) {

    int skipped = 0;

    while (i!=end) {

        if (!strcmp(i->constData(), gyro::FIRST_LINE)) {
            return;
        }

        ++i;
        ++skipped;
    }

    msg += "failed to find the first line!";

    throw '\0';
}

void Solver::skip_line(const char* text, li& i, cli& end) {

    if (i==end) {
        msg += "unexpected end of output when checking ";
        msg += text;
        msg += "!";
    }
    else if (strcmp(i->constData(), text)) {
        msg += "expected explanatory comment: ";
        msg += text;
        msg += ", found: ";
        msg +=  i->constData();
        msg += "!";
    }
    else {
        ++i;
        cout << text << endl;
        return;
    }

    throw '\0';
}

void Solver::echo_line(const char* text, li& i, cli& end) {

    skip_line(text, i, end);

    if (i==end) {
        msg += "unexpected end of output!";
        throw '\0';
    }

    cout << i->constData() << endl;

    ++i;
}

void Solver::read_all_lines(li &i, cli &end) {

    skip_irrelevant_lines(i, end);

    echo_line(gyro::FIRST_LINE, i, end);

    echo_line(gyro::CONFIG_FILE_ID, i, end);

    // TODO Copy error value
    echo_line(gyro::ERROR_IN_G, i, end);

    // TODO Check if matches the hard-coded value
    echo_line(gyro::NUMBER_OF_VARS, i, end);

    const int n_vars = gyro::NUMBER_OF_VARIABLES;

    // TODO Check acceptance level
    read_vector(gyro::SOLUTION_VECTOR, x, n_vars, i, end);

    read_vector(gyro::VARIABLE_LOWER_BOUNDS, x_lb, n_vars, i, end);

    read_vector(gyro::VARIABLE_UPPER_BOUNDS,  x_ub, n_vars, i, end);

    // TODO Check value
    echo_line(gyro::NUMBER_OF_SAMPLES, i, end);

    //=======================================================

    n = n_samples();

    const int n_elem = 9*n;

    m = new double[n_elem];

    read_vector(gyro::ROTATION_MATRICES, m, n_elem, i, end);

    skip_line(gyro::END_OF_FILE, i, end);

    msg = "Successfully finished!";
}

bool Solver::read_results() {

    if ((n!=0) || (m!=0))
        throw logic_error("The rotation matrices should have been released!");

    bool result = SUCCESS;

    QList<QByteArray> arr( solver->readAll().split('\n') );

    li i = arr.begin();

    cli end = arr.end();

    try {

        read_all_lines(i, end);;
    }
    catch (char ) {

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
    else if (exitCode == ERROR_CONVERGENCE) {
        msg += "regression failed to converge!";
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
    msg = "Error: ";

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

// TODO Access elements through enum?
double Solver::R(int sample, int i, int j) const {

    MutexUnlocker mu(mutex);

    if ((n==0) || (m==0)) {
        throw logic_error("Run the solver first!");
    }

    if ((sample<0) || (sample>=n) || (i<1) || (i>3) || (j<1) || (j>3)) {
        throw range_error("Index out of range for rotation matrix!");
    }

    const int index = 9*sample + 3*(i-1) + (j-1);

    return m[index];
}

Solver::~Solver() {

    cleanup_all();

    delete mutex;
}

}
