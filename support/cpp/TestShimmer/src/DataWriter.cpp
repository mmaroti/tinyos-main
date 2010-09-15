#ifndef DATAWRITER_CPP
#define DATAWRITER_CPP

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "QProcess"
#include "DataWriter.hpp"
#include "DataWriteException.hpp"
#include "CompileTimeConstants.hpp"

using namespace std;

namespace ipo {

DataWriter::DataWriter(QProcess* proc, const char* dump_to_file) :
        process(proc), ofs(dump_to_file?(new ofstream(dump_to_file)):0)
{

}

void DataWriter::writeAll() {

    write_line(gyro::NUMBER_OF_SAMPLES);

    write_n_samples();

    write_line(gyro::INPUT_DATA);

    write_samples();
}

void DataWriter::write_data(double data[SIZE]) {

    ostringstream os;

    os << setprecision(16) << scientific;

    for (int i=0; i<SIZE; ++i) {

        os << data[i] << '\n';
    }

    os << flush;

    write(os.str().c_str());
}

void DataWriter::write_samples() {

    double data[SIZE];

    const int n = n_samples();

    for (int i=0; i<n; ++i) {

        at(i, data);

        write_data(data);
    }
}

void DataWriter::write_n_samples() {

    const int n = n_samples();

    if (n<1)
        throw logic_error("Incorrect number of samples!");

    ostringstream os;

    os << n << '\n' << flush;

    write(os.str().c_str());
}

void DataWriter::write_line(const char* text) {

    string line(text);

    line.push_back('\n');

    write(line.c_str());
}

void DataWriter::write(const char *data) {

    if (ofs) {
        (*ofs) << data << flush;
    }

    int k = process->write(data);

    if (k==-1) {
        throw DataWriteException();
    }
}

DataWriter::~DataWriter() {

    delete ofs;
}

}

#endif
