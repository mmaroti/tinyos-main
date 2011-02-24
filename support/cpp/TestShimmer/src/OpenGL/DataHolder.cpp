/* Copyright (c) 2011 University of Szeged
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

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "DataHolder.hpp"

using namespace std;

enum {
    FLEX_MIN,
    FLEX_MAX,
    SUP_MIN,
    SUP_MAX,
    PRON_MIN,
    PRON_MAX,
    LAT_MIN,
    LAT_MAX,
    MED_MIN,
    MED_MAX,
    SIZE
};

// FIXME Knows sampling rate
DataHolder::DataHolder(ElbowFlexSign s) : sign(s), SAMPLING_RATE(204.8), out(new ostringstream) {

    rotation_matrices = flexion = supination = deviation = 0;
    size = 0;

    extrema = new double[SIZE];

    flex_range = "Flex  ";
    sup_range  = "Sup  ";
    pron_range = "Pron  ";
    lat_range  = "Lat Dev  ";
    med_range  = "Med Dev  ";
}

DataHolder::~DataHolder() {

    delete out;

    delete[] rotation_matrices;

    delete[] flexion;
    delete[] supination;
    delete[] deviation;

    delete[] extrema;
}

void DataHolder::check_index(int i) const {

    if (i<0 || i>=size) {

        out->str("");

        *out << "index: " << i << ", valid range [0, " << size << ")" << flush;

        throw out_of_range(out->str());
    }
}

ostringstream& DataHolder::get_out() const {

    out->str("");

    *out << fixed << setprecision(1);

    return *out;
}

void DataHolder::grab_content(const char *filename) {

    ifstream in;

    in.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

    in.open(filename);

    in >> size;

    const int n_elem = 9*size;

    rotation_matrices = new double[n_elem];

    for (int i=0; i<n_elem; ++i) {

        in >> rotation_matrices[i];
    }

    find_min_max();
}

void DataHolder::set_content(double* rotmat, int length) {

    size = length;

    rotation_matrices = rotmat;

    find_min_max();
}

int DataHolder::number_of_samples() const {

    return size;
}

void DataHolder::init_angle_arrays() {


    if (flexion || supination || deviation) {

        throw logic_error("already initialized");
    }

    if (size<1) {

        throw logic_error("there should be at least 1 sample");
    }

    flexion    = new double[size];
    supination = new double[size];
    deviation  = new double[size];
}

void DataHolder::fill_angle_arrays() {

    for (int i=0; i<size; ++i) {

        flexion[i]    = flexion_deg(i);
        supination[i] = supination_deg(i);
        deviation[i]  = deviation_deg(i);
    }
}

const double* DataHolder::matrix_at(int i) const {

    check_index(i);

    return rotation_matrices + (9*i);
}

void DataHolder::find_min_max() {

    init_angle_arrays();
    fill_angle_arrays();

    extrema[FLEX_MIN] = *min_element(flexion, flexion+size);
    extrema[FLEX_MAX] = *max_element(flexion, flexion+size);

    extrema[SUP_MIN] = *min_element(supination, supination+size);
    extrema[SUP_MAX] = *max_element(supination, supination+size);

    extrema[LAT_MIN] = *min_element(deviation, deviation+size);
    extrema[LAT_MAX] = *max_element(deviation, deviation+size);

    set_pronation();
    set_med_dev();
    save_ranges();
}

void DataHolder::set_pronation() {

    if (extrema[SUP_MAX] < 0) {

        extrema[PRON_MIN] = -extrema[SUP_MAX];
        extrema[PRON_MAX] = -extrema[SUP_MIN];

        extrema[SUP_MIN]  = extrema[SUP_MAX] = 0.0;
    }
    else if (extrema[SUP_MIN] < 0) {

        extrema[PRON_MAX] = -extrema[SUP_MIN];

        extrema[SUP_MIN] = extrema[PRON_MIN] = 0.0;
    }
    else {

        extrema[PRON_MIN] = extrema[PRON_MAX] = 0.0;
    }
}

// TODO Eliminate duplication -- would make the code more difficult to understand
void DataHolder::set_med_dev() {

    if (extrema[LAT_MAX] < 0) {

        extrema[MED_MIN] = -extrema[LAT_MAX];
        extrema[MED_MAX] = -extrema[LAT_MIN];

        extrema[LAT_MIN] = extrema[LAT_MAX] = 0.0;
    }
    else if (extrema[LAT_MIN] < 0) {

        extrema[MED_MAX] = -extrema[LAT_MIN];

        extrema[MED_MIN] = extrema[LAT_MIN] = 0.0;

    }
    else {

        extrema[MED_MIN] = extrema[MED_MAX] = 0.0;
    }
}

void DataHolder::save_ranges() {

    save_flex();

    save_sup();

    save_pron();

    save_lat_dev();

    save_med_dev();
}

string str(double x) {

    ostringstream os;

    os << fixed << setprecision(1);

    os << x << flush;

    return os.str();
}

void DataHolder::save_flex() {

    flex_range += str(flexion[0]);

    flex_range += range(FLEX_MIN, FLEX_MAX);
}

void DataHolder::save_sup() {

    sup_range += str(supination[0] >= 0 ? supination[0] : 0.0);

    sup_range += range(SUP_MIN, SUP_MAX);
}

void DataHolder::save_pron() {

    pron_range += str(supination[0] < 0 ? -supination[0] : 0.0);

    pron_range += range(PRON_MIN, PRON_MAX);
}

void DataHolder::save_lat_dev() {

    lat_range += str(deviation[0] >= 0 ? deviation[0] : 0.0);

    lat_range += range(LAT_MIN, LAT_MAX);
}

void DataHolder::save_med_dev() {

    med_range += str(deviation[0]< 0 ? -deviation[0] : 0.0);

    med_range += range(MED_MIN, MED_MAX);
}

typedef ostringstream& oss;

const string DataHolder::range(int MIN, int MAX) {

    oss os = get_out();

    os << " / " << extrema[MIN] << " / " << extrema[MAX] << " / ";
    os << extrema[MAX]-extrema[MIN] << " deg" << flush;

    return os.str();
}

namespace {

    enum {
            R11, R12, R13,
            R21, R22, R23,
            R31, R32, R33
    };

    const double RAD2DEG = 57.2957795131;
    const double PI_HALF = 1.57079632679;
}

double DataHolder::flexion_deg(int i) const {

    const double* const m = matrix_at(i);

    return (atan2(m[R31], m[R21])+PI_HALF)*RAD2DEG;
}

double DataHolder::supination_deg(int i) const {

    const double* const m = matrix_at(i);

    return atan2(m[R12], m[R13])*RAD2DEG*(sign.sup);
}

double DataHolder::deviation_deg(int i) const {

    const double* const m = matrix_at(i);

    // Sign: to make lateral-medial correct for right arm
    return (acos(m[R11])-PI_HALF)*RAD2DEG*(sign.dev);
}

const string DataHolder::flex(int i) const {

    check_index(i);

    oss os = get_out();

    os << "Flex " << flexion[i] << " deg";

    return os.str();
}

const string DataHolder::sup(int i) const {

    check_index(i);

    oss os = get_out();

    if (supination[i] >= 0) {

        os << "Sup " <<   supination[i] << " deg";
    }
    else {

        os << "Pron " << -supination[i] << " deg";
    }

    return os.str();
}

const string DataHolder::dev(int i) const {

    check_index(i);

    oss os = get_out();

    if (deviation[i] > 0) {

        os << "Lat Dev " <<  deviation[i] << " deg";
    }
    else {

        os << "Med Dev " << -deviation[i] << " deg";
    }

    return os.str();
}

const string DataHolder::time(int i) const {

    check_index(i);

    oss os = get_out();

    double sec = i/SAMPLING_RATE;

    os << "Time: " << sec << " s" << flush;

    return os.str();
}

const char* DataHolder::flex_info() const {

    return flex_range.c_str();
}

const char* DataHolder::sup_info()  const {

    return sup_range.c_str();
}

const char* DataHolder::pron_info() const {

    return pron_range.c_str();
}

const char* DataHolder::lat_info()  const {

    return lat_range.c_str();
}

const char* DataHolder::med_info()  const {

    return med_range.c_str();
}
