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

#ifndef INPUTDATA_HPP_
#define INPUTDATA_HPP_

namespace gyro {

class input {

public:

	input(	const double* acc_x,
			const double* acc_y,
			const double* acc_z,
			const double* wx,
			const double* wy,
			const double* wz,
			int N,
			double dt,
			double g_ref);

	const double* acc_x() const { return acc_x_; }
	const double* acc_y() const { return acc_y_; }
	const double* acc_z() const { return acc_z_; }

	const double* wx() const { return wx_; }
	const double* wy() const { return wy_; }
	const double* wz() const { return wz_; }

	int N() const { return N_; }

	double dt() const    { return dt_; }
	double g_ref() const { return g_ref_; }

	~input();

private:

	input(const input& other);
	input& operator=(const input& );

	const double* const acc_x_;
	const double* const acc_y_;
	const double* const acc_z_;

	const double* const wx_;
	const double* const wy_;
	const double* const wz_;

	const int N_;

	const double dt_;
	const double g_ref_;
};

}

#endif
