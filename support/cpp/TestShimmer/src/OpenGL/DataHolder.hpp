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

#ifndef DATAHOLDER_HPP
#define DATAHOLDER_HPP

#include <iosfwd>
#include <string>
#include "ElbowFlexSign.hpp"

class DataHolder {

public:

    static DataHolder* right();

    static DataHolder* left();

    void grab_content(const char* filename);

    void set_content(double* rotmat, int size);

    int number_of_samples() const;

    const char* flex_info() const;
    const char* sup_info()  const;
    const char* pron_info() const;
    const char* lat_info()  const;
    const char* med_info()  const;

    const double* matrix_at(int i) const;

    const std::string flex(int i) const;
    const std::string sup(int i)  const;
    const std::string dev(int i)  const;

    const std::string time(int i) const;

    ~DataHolder();

private:

    DataHolder(ElbowFlexSign sign);

    DataHolder(const DataHolder& );
    DataHolder& operator=(const DataHolder& );

    void init_angle_arrays();
    void fill_angle_arrays();
    void find_min_max();

    void set_pronation();
    void set_med_dev();
    void save_ranges();
    void save_flex();
    void save_sup();
    void save_pron();
    void save_lat_dev();
    void save_med_dev();
    const std::string range(int MIN, int MAX);

    std::ostringstream& get_out() const;
    void check_index(int i) const;

    double flexion_deg(int i) const;
    double supination_deg(int i) const;
    double deviation_deg(int i) const;

    const ElbowFlexSign sign;
    const double SAMPLING_RATE;

    std::ostringstream* const out;

    int size;
    double* rotation_matrices;

    double* flexion;
    double* supination;
    double* deviation;
    double* extrema;

    std::string flex_range;
    std::string sup_range;
    std::string pron_range;
    std::string lat_range;
    std::string med_range;

};

#endif // DATAHOLDER_HPP
