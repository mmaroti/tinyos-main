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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "DataHolder.hpp"
#include "MatrixVector.hpp"

using namespace std;

DataHolder* DataHolder::right(double* rotmat, const int length) {

    return new DataHolder(ElbowFlexSign::right(), rotmat, length);
}

DataHolder* DataHolder::left(double* rotmat, const int length) {

    return new DataHolder(ElbowFlexSign::left(), rotmat, length);
}

// FIXME Knows sampling rate
DataHolder::DataHolder(ElbowFlexSign s, double* rotmat, const int length)
    : sign(s), rotation_matrices(rotmat), size(length), SAMPLING_RATE(204.8), out(new ostringstream)
{

    flexion = supination = deviation = 0;

    extrema = new double[SIZE_OF_ARRAY];

    flex_range = "Flex  ";
    sup_range  = "Sup  ";
    pron_range = "Pron  ";
    lat_range  = "Lat Dev  ";
    med_range  = "Med Dev  ";

    make_dev_zero_at_horizontal_crossing();
    find_min_max();
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

const double* DataHolder::min_max() const {

    return extrema;
}

int DataHolder::number_of_samples() const {

    return size;
}

void DataHolder::init_angle_arrays() {


    if (flexion || supination || deviation) {

        throw logic_error("already initialized");
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

    sup_range += str(sup_begin());

    sup_range += range(SUP_MIN, SUP_MAX);
}

void DataHolder::save_pron() {

    pron_range += str(pron_begin());

    pron_range += range(PRON_MIN, PRON_MAX);
}

void DataHolder::save_lat_dev() {

    lat_range += str(lat_begin());

    lat_range += range(LAT_MIN, LAT_MAX);
}

void DataHolder::save_med_dev() {

    med_range += str(med_begin());

    med_range += range(MED_MIN, MED_MAX);
}

double DataHolder::sup_begin() const {

    return supination[0] >= 0 ? supination[0] : 0.0;
}

double DataHolder::pron_begin() const {

    return supination[0] < 0 ? -supination[0] : 0.0;
}

double DataHolder::lat_begin() const {

    return deviation[0] >= 0 ? deviation[0] : 0.0;
}

double DataHolder::med_begin() const {

    return deviation[0] < 0 ? -deviation[0] : 0.0;
}

typedef ostringstream& oss;

const string DataHolder::range(int MIN, int MAX) {

    oss os = get_out();

    os << " / " << extrema[MIN] << " / " << extrema[MAX] << " / ";
    os << extrema[MAX]-extrema[MIN] << " deg" << flush;

    return os.str();
}

const string DataHolder::angles_in_csv() const {

    ostringstream os;

    os << setprecision(4) << scientific;

    const char sep = ';';

    os <<   flexion[0] << sep << extrema[FLEX_MIN] << sep << extrema[FLEX_MAX] << sep;
    os <<  sup_begin() << sep << extrema[ SUP_MIN] << sep << extrema[ SUP_MAX] << sep;
    os << pron_begin() << sep << extrema[PRON_MIN] << sep << extrema[PRON_MAX] << sep;
    os <<  med_begin() << sep << extrema[ MED_MIN] << sep << extrema[ MED_MAX] << sep;
    os <<  lat_begin() << sep << extrema[ LAT_MIN] << sep << extrema[ LAT_MAX] << flush;


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

void DataHolder::make_dev_zero_at_horizontal_crossing() {

    if (size < 2) {

        throw logic_error("there should be at least 2 samples");
    }

    const int i = find_horizontal_plane_crossing();

    if (i < size) {

        const double angle = angle_making_deviation_zero(i);

        rotate_all_around_y_axis(angle);
    }
}

int  DataHolder::find_horizontal_plane_crossing() const {

    int i = 1; // caller assumes i > 0

    for ( ; i<size; ++i) {

        if (y_gl_coordinate(i) > 0) {

            break;
        }
    }

    return i;
}

double DataHolder::y_gl_coordinate(int i) const {

    return matrix_at(i)[R31];
}

double DataHolder::angle_making_deviation_zero(int i) const {

    const double* const m1 = matrix_at(i-1);
    const double* const m2 = matrix_at(i);

    const double x_gl_coord = (m1[R21]+m2[R21])/2;
    const double z_gl_coord = (m1[R11]+m2[R11])/2;

    double dev = atan2(-z_gl_coord,x_gl_coord);

    cout << "Deviation at crossing horizontal plane: ";
    cout << fixed << setprecision(2) << dev*RAD2DEG << " deg" << endl;

    return -dev;
}

void DataHolder::rotate_all_around_y_axis(const double angle) {

    using namespace gyro;

    double m[9];

    m[R11] = cos(angle); m[R12] = -sin(angle); m[R13] = 0.0;
    m[R21] = sin(angle); m[R22] =  cos(angle); m[R23] = 0.0;
    m[R31] = 0.0;        m[R32] =  0.0;        m[R33] = 1.0;

    const matrix3 correction(m);

    for (int i=0; i<size; ++i) {

        double* R_old = const_cast<double*> (matrix_at(i));

        matrix3 R(R_old);

        matrix3 R_new = correction*R;

        R_new.copy_to(R_old);
    }
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
