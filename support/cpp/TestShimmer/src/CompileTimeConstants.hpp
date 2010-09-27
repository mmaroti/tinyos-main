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

#ifndef COMPILETIMECONSTANTS_HPP_
#define COMPILETIMECONSTANTS_HPP_

namespace gyro {

const int NUMBER_OF_VARIABLES(12);

enum {
        SUCCESS = 0,
        ERROR_READING_CONFIG = 11,
        ERROR_READING_INPUT,
        ERROR_INCORRECT_TIMESTAMP,
        ERROR_INITIALIZATION,
        ERROR_NUMBER_INVALID,
        ERROR_OPTIMIZATION,
        ERROR_INVALID_OPTION,
        ERROR_UNKNOWN
};

const int CONFIG_FILE_NOT_FOUND = -1;

const int CONFIG_FILE_HAS_NO_ID = 0;

const char FIRST_LINE[] = "# First line after the output of IPOPT";

const char BUILD_ID[] = "# Solver built on " __DATE__ " " __TIME__;

const char CONFIG_FILE_ID[] = "# ID of configuration file";

const char ERROR_IN_G[] = "# Error in g (in m/s^2)";

const char NUMBER_OF_VARS[] = "# Number of variables";

const char SOLUTION_VECTOR[] = "# Solution vector";

const char VARIABLE_LOWER_BOUNDS[] = "# Lower bounds of the variables";

const char VARIABLE_UPPER_BOUNDS[] = "# Upper bounds of the variables";

const char NUMBER_OF_SAMPLES[] = "# Number of samples";

const char ROTATION_MATRICES[] = "# Rotation matrices row-wise";

const char END_OF_FILE[] = "# End of file";

const char INPUT_DATA[] = "# Input data: timestamp, accel_{x,y,z}, gyro_{x,y,z}";

}

#endif
