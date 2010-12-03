/* Copyright (c) 2010, University of Szeged
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
*      Author: Ali Baharev
*/

#include <iostream>
#include "TimeSyncCalc.hpp"
#include "TimeSyncMerger.hpp"
#include "LinearEquations.h"

using namespace std;

namespace sdc {

TimeSyncCalc::TimeSyncCalc(int mote, int reboot)
    : time_sync(new TimeSyncMerger(mote, reboot))
{
    const Map& pairs = time_sync->pairs();

    Map::const_iterator i = pairs.begin();

    while (i != pairs.end()) {

        cout << "Processing " << i->first << endl;

        compute_skew_offset(i->second);

        ++i;
    }
}
void TimeSyncCalc::compute_skew_offset(const vector<Pair>& sync_points) const {

    LinearEquations lin_eq;

    lin_eq.getVariable("skew_1");
    lin_eq.getVariable("offset");

    const int n = static_cast<int>(sync_points.size());

    for (int i=0; i<n; ++i) {

        add_equation(lin_eq, sync_points.at(i));
    }

    lin_eq.printStatistics();

    Solution* solution = lin_eq.solveWithSVD(0.0);

    solution->print();
}

void TimeSyncCalc::add_equation(LinearEquations& lin_eq, const Pair& pair) const {

    Equation* eq = lin_eq.createEquation();

    // t1    = skew  *t2 + offset
    // t1-t2 = skew_1*t2 + offset
    eq->setConstant(pair.first-pair.second);
    eq->setCoefficient("skew_1",   pair.second);
    eq->setCoefficient("offset", 1.0);

    lin_eq.addEquation(eq);
}

TimeSyncCalc::~TimeSyncCalc() {
    // Do NOT remove this empty dtor: required to generate the dtor of auto_ptr
}

}
