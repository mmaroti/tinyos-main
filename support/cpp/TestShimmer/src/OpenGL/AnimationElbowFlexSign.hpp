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

#ifndef ANIMATIONELBOWFLEXSIGN_HPP
#define ANIMATIONELBOWFLEXSIGN_HPP

// FIXME Where is GLenum defined?
#include <QGLWidget>

class AnimationElbowFlexType {

public:

    static AnimationElbowFlexType right() {

        return AnimationElbowFlexType(-1, 4.5, -5.0);
    }

    static AnimationElbowFlexType left() {

        return AnimationElbowFlexType( 1, 6.5, -3.5);
    }

    const int sign;
    const GLenum handFront;
    const GLenum handBack;
    const double frontShift;
    const double planShift;

private:

    AnimationElbowFlexType(int sign, double frontShift, double planShift) :
            sign(sign),
            handFront(sign==-1?GL_FILL:GL_LINE),
            handBack( sign==-1?GL_LINE:GL_FILL),
            frontShift(frontShift),
            planShift(planShift)
    { }
};

#endif // ANIMATIONELBOWFLEXSIGN_HPP
