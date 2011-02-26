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

#ifndef GLELBOWFLEXWIDGET_HPP
#define GLELBOWFLEXWIDGET_HPP

#include <QGLWidget>
#include "AnimationElbowFlexSign.hpp"

class DataHolder;

class GLElbowFlexWidget : public QGLWidget
{
    Q_OBJECT

public:

    static GLElbowFlexWidget* right(double* rotmat, int size);

    static GLElbowFlexWidget* left(double* rotmat, int size);

    int numberOfSamples() const;

    void setFrame(int pos);

    ~GLElbowFlexWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:

    void clicked();

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent* event);

private:

    GLElbowFlexWidget(AnimationElbowFlexSign sign, DataHolder* dataHolder, double* rotmat, int size);

    void rotate();

    void reset();

    void setCameraPosition();
    void setState();

    void sideView();
    void planView();
    void frontView();

    void headSilhouette(GLUquadricObj* qobj);
    void headSolid(GLUquadricObj* qobj);

    void sideHead();
    void planHead();
    void frontHead();

    void writeData();

    void drawLinearParts();

    void drawIrrelevantParts();
    void shoulder();
    void neck();
    void leftUpperArm();

    void body();

    void drawRightArm();
    void upperArm();
    void elbow();
    void rotateForeArm();
    void foreArm();
    void hand();

    const AnimationElbowFlexSign type;

    DataHolder* const data;

    GLfloat rotmat[16];

    GLuint list;

    int position;
    int size;
};

#endif // GLELBOWFLEXWIDGET_HPP
