/* Copyright (c) 2010, 2011 University of Szeged
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

#include <assert.h>
#include <iostream>
#include <stdint.h>
#include "TimeSyncCalc.hpp"
#include "TimeSyncMerger.hpp"
#include "LinearEquations.h"

using namespace std;

namespace sdc {

TimeSyncCalc::TimeSyncCalc(const TimeSyncMerger& merger, const RecordID& rid, TimeSyncData* data, int length)
{
    const Map& pairs = merger.pairs();

    assert(static_cast<int>(pairs.size())==length);

    Map::const_iterator i = pairs.begin();

    int k = 0;

    while (i != pairs.end()) {

        RecordPairID p = i->first;

        cout << "Processing " <<  p << endl;

        swap_pairs = p.isFirst(rid);

        data[k++] = compute_skew_offset(i->second);

        ++i;
    }
}

const TimeSyncData TimeSyncCalc::compute_skew_offset(const vector<Pair>& sync_points) const {

    LinearEquations lin_eq;

    lin_eq.getVariable("skew_1");
    lin_eq.getVariable("offset");

    const int n = static_cast<int>(sync_points.size());

    for (int i=0; i<n; ++i) {

        add_equation(lin_eq, sync_points.at(i));
    }

    //lin_eq.printStatistics();

    Solution* solution = lin_eq.solveWithSVD(0);

    const double skew_1 = solution->getValue("skew_1");
    const double offset = solution->getValue("offset");

    cout << "skew_1: " << skew_1 << ", offset: " << offset << endl;

    cout << "average error " << solution->getAverageError() << endl;
    cout << "maximum error " << solution->getMaximumError() << endl;

    return TimeSyncData(skew_1, offset);
}

void TimeSyncCalc::add_equation(LinearEquations& lin_eq, const Pair& pair) const {

    Equation* eq = lin_eq.createEquation();

    // t1    = skew  *t2 + offset
    // t1-t2 = skew_1*t2 + offset

    int64_t t1 = pair.first;
    int64_t t2 = pair.second;
    int sign = 1;

    if (swap_pairs) {
        int64_t tmp = t1;
        t1 = t2;
        t2 = tmp;
        sign = -1;
    }

    eq->setConstant(t1-t2);
    eq->setCoefficient("skew_1", sign*t2);
    eq->setCoefficient("offset", sign);

    lin_eq.addEquation(eq);
}

}
