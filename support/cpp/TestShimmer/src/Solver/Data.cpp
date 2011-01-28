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

#include <stdexcept>
#include <sstream>
#include <QDebug>
#include "DataRecorder.h"

using namespace std;

// FIXME Eliminate this hideous workaround by the Singleton pattern

DataRecorder* dr = 0;

// Assumes singleton, and being called from a single thread

namespace ipo {

int begin_mark = -1;
int end_mark   = -2;
int rec_size   = -9;

void check_validity(int begin, int end) {

    rec_size = dr->size();

    if (begin < 0 || begin >= end || end > rec_size) {
        ostringstream os;
        os << __FILE__ << " begin, end, size: " << begin << '\t' << end << '\t' << rec_size << flush;
        throw logic_error(os.str());
    }
}

void set_markers(int begin, int end) {

    check_validity(begin, end);

    begin_mark = begin;
    end_mark   = end;
}

void mark_all() {

    set_markers(0, dr->size());
}

void clear_markers() {

    begin_mark = -1;
    end_mark   = -2;
    rec_size   = -9;
}

int n_samples() {

    if (begin_mark<0 || begin_mark>=end_mark) {
        qDebug() << "begin, end: " << begin_mark << end_mark;
        throw logic_error("call set_markers first");
    }

    if (rec_size != dr->size()) {
        throw logic_error("sample size changed");
    }

    return end_mark-begin_mark;
}

void at(int i, double data[SIZE]) {

    dr->at(i+begin_mark, data);
}

void load(const Results &r) {

    dr->loadResults(r);
}

}
