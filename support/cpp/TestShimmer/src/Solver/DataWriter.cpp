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

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "DataWriter.hpp"
#include "DataWriteException.hpp"
#include "CompileTimeConstants.hpp"

using namespace std;

namespace ipo {

DataWriter::DataWriter() : out(new ofstream()) {

    out->exceptions(ofstream::failbit | ofstream::badbit);
}

DataWriter::~DataWriter() {

    delete out;
}

void DataWriter::writeAll(const char* filename) {

    const int n = n_samples();

    if (n<1)
        throw logic_error("Incorrect number of samples!");

    try {

        out->open(filename);

        *out << gyro::NUMBER_OF_SAMPLES << '\n';

        *out << n << '\n';

        *out << gyro::INPUT_DATA << '\n';

        write_samples(n);

        out->close();

    }
    catch (ios_base::failure& )  {

        throw DataWriteException();
    }
}

void DataWriter::write_data(double data[SIZE]) {

    for (int i=0; i<SIZE; ++i) {

        *out << data[i] << '\n';
    }
}

void DataWriter::write_samples(int n) {

    double data[SIZE];

    *out << setprecision(16) << scientific;

    for (int i=0; i<n; ++i) {

        at(i, data);

        write_data(data);
    }
}

}
