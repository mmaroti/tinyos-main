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

#ifndef PATHLENGTH_HPP_
#define PATHLENGTH_HPP_

#include <ostream>
#include "InputData.hpp"
#include "CompileTimeConstants.hpp"

namespace gyro {

template<typename T>
class PathLength {

public:

	PathLength(const double* rotmat, const Input& data, std::ostream& os) :
		R(rotmat),
		time_stamp(data.time_stamp()),
		a_x(data.acc_x()),
		a_y(data.acc_y()),
		a_z(data.acc_z()),
		N(data.N()),
		g_ref(data.g_ref()),
		log(os)
	{
		logging = false;
	}

	T f(const T* x)  {

		init(x);

		for (int i=1; i<N; ++i) {
			update_path_length(i);
		}

		return s;
	}

	void verbose() {
		logging = true;
	}

private:

	void init(const T* const x) {

		v[X] = v[Y] = v[Z] = 0.0;
		r[X] = r[Y] = r[Z] = 0.0;
		s = 0.0;

		for (int i=0, k=0; i<3; ++i) {
			for (int j=0; j<3; ++j) {
				C[i][j] = x[k++];
			}
		}

/*		for (int i=0; i<3; ++i) {
			for (int j=0; j<3; ++j) {
				C[i][j] = 0.0;
			}
		}
*/
		C[0][0] += 1.0;
		C[1][1] += 1.0;
		C[2][2] += 1.0;

		d[X] = x[ 9];
		d[Y] = x[10];
		d[Z] = x[11];
	}

	void update_path_length(int sample) {

		T am[3];

		for (int i=0; i<3; ++i) {
			am[i] = C[i][0]*a_x[sample] + C[i][1]*a_y[sample] + C[i][2]*a_z[sample] + d[i];
		}

		const int p = 9*sample;

		for (int i=0; i<3; ++i) {
			const int k = p + 3*i;
			a[i] = R[k]*am[X]+R[k+1]*am[Y]+R[k+2]*am[Z];
		}

		a[2] -= g_ref;

		//log << "====================================================" << endl;

		for (int i=0; i<3; ++i) {
			//log << "a[" << i << "] = " << a[i] << std::endl;
		}

		const double dt = (time_stamp[sample]-time_stamp[sample-1])/TICKS_PER_SEC;

		v[X] += (a[X]*dt);
		v[Y] += (a[Y]*dt);
		v[Z] += (a[Z]*dt);

		for (int i=0; i<3; ++i) {
			//log << "v[" << i << "] = " << v[i] << std::endl;
		}

		r[X] += (v[X]*dt);
		r[Y] += (v[Y]*dt);
		r[Z] += (v[Z]*dt);

		T v_len = sqrt(pow(v[X], 2) + pow(v[Y], 2) + pow(v[Z], 2));

		s += v_len*dt;

		if (logging) {
			write(sample);
		}

		//log << "s = " << s << std::endl;
	}

	void write(int i) {
		log << i;
		log << '\t' << a_x[i] << '\t' << a_y[i] << '\t' << a_z[i];
		log << '\t' << a[X] << '\t' << a[Y] << '\t' << a[Z];
		log << '\t' << v[X] << '\t' << v[Y] << '\t' << v[Z];
		log << '\t' << r[X] << '\t' << r[Y] << '\t' << r[Z] << '\t' << s;
		log << '\n' << std::flush;
	}

	PathLength& operator=(const PathLength& );

	const double* const R;
	const double* const time_stamp;
	const double* const a_x;
	const double* const a_y;
	const double* const a_z;
	const int N;
	const double g_ref;
	std::ostream& log;
	bool logging;

	T C[3][3];
	T d[3];
	T a[3];
	T v[3];
	T r[3];
	T s;

	enum { X, Y, Z };
};

}

#endif

