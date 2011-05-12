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

#ifndef ARMWIDGET_HPP
#define ARMWIDGET_HPP

#include <map>
#include <string>
#include <vector>
#include <QGLWidget>
#include "AnimationElbowFlexSign.hpp"
#include "MatrixVector.hpp"

class ArmWidget : public QGLWidget
{
    Q_OBJECT

public:

    static ArmWidget* right();

    static ArmWidget* left();

    void display(const std::map<int,gyro::matrix3>& matrices, const std::vector<std::string>& text);

    void setReference(const std::vector<double>& headings);

    ~ArmWidget();

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

    ArmWidget(AnimationElbowFlexType sign);

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
    void stillUpperArm();

    void body();

    void drawMovingArm();
    void rotateToUpperArmRef();
    void upperArm();
    void moveToElbow();
    void elbow();
    void rotateToForeArmRef();
    void applyForeArmRotation();
    void foreArm();
    void hand();

    void updateMatrix(GLfloat mat[16], const gyro::matrix3& rotationMatrix);

    enum TEXT_POSITION {
        FLEX_LABEL,
        SUP_LABEL,
        DEV_LABEL,
        FRAME_LABEL,
        FLEX_LINE,
        EXT_LINE,
        SUP_LINE,
        PRON_LINE,
        LAT_LINE,
        MED_LINE,
        SIZE_OF_TEXT
    };

    const char* text(TEXT_POSITION pos) const;

    const AnimationElbowFlexType type;

    GLfloat foreArmMat[16];
    GLfloat upperArmMat[16];

    GLuint list;

    double foreArmRefHeading;
    double upperArmRefHeading;

    std::vector<std::string> labelAndTable;
};

#endif // ARMWIDGET_HPP
