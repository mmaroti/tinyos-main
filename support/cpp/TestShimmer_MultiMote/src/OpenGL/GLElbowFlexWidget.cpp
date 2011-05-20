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

#include <stdexcept>
#include <QtGui>
#include <QtOpenGL>
#include <QDebug>
#include "GLElbowFlexWidget.hpp"
#include "DataHolder.hpp"

namespace {

    enum {
            R11, R12, R13,
            R21, R22, R23,
            R31, R32, R33
    };

    enum {
            M11 = 0, M12 = 4, M13 =  8, M14 = 12,
            M21 = 1, M22 = 5, M23 =  9, M24 = 13,
            M31 = 2, M32 = 6, M33 = 10, M34 = 14,
            M41 = 3, M42 = 7, M43 = 11, M44 = 15
    };

    enum {
        SOLID_DISK,
        SILHOUETTE,
        LIST_LENGTH
    };

    const int LINE_WIDTH = 3;
}

GLElbowFlexWidget* GLElbowFlexWidget::right(double* rotmat, int size) {

    return new GLElbowFlexWidget(AnimationElbowFlexType::right(), DataHolder::right(rotmat, size));
}

GLElbowFlexWidget* GLElbowFlexWidget::left(double* rotmat, int size) {

    return new GLElbowFlexWidget(AnimationElbowFlexType::left(), DataHolder::left(rotmat, size));
}

GLElbowFlexWidget::GLElbowFlexWidget(AnimationElbowFlexType sign,
                                     DataHolder *dataHolder )
: type(sign),
  data(dataHolder)
{
    for (int i=0;i<16; ++i)
        rotmat[i] = (GLfloat) 0.0;

    rotmat[M11] = rotmat[M22] = rotmat[M33] = rotmat[M44] = (GLfloat) 1.0;

    size = data->number_of_samples();

    position = 0;
}

GLElbowFlexWidget::~GLElbowFlexWidget() {

    delete data;
}

QSize GLElbowFlexWidget::minimumSizeHint() const {

    return QSize(300, 300);
}

QSize GLElbowFlexWidget::sizeHint() const {

    return QSize(750, 750);
}

// TODO Threading?
void GLElbowFlexWidget::rotate() {

    const double* mat = data->matrix_at(position);

    rotmat[M31] = (GLfloat) mat[R11];
    rotmat[M21] = (GLfloat) mat[R31];
    rotmat[M11] = (GLfloat) mat[R21];

    rotmat[M32] = (GLfloat) mat[R12];
    rotmat[M22] = (GLfloat) mat[R32];
    rotmat[M12] = (GLfloat) mat[R22];

    rotmat[M33] = (GLfloat) mat[R13];
    rotmat[M23] = (GLfloat) mat[R33];
    rotmat[M13] = (GLfloat) mat[R23];

    updateGL();
}

// TODO Can we pass a member function to GLU?
void APIENTRY errorCallback() {

    const GLubyte* estring = gluErrorString(glGetError());

    printf("OpenGL Error: %s\n", estring);

    exit(0);
}

void GLElbowFlexWidget::headSilhouette(GLUquadricObj* qobj) {

    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);

    gluQuadricNormals(qobj, GLU_NONE);

    glNewList(list+SILHOUETTE, GL_COMPILE);

        gluDisk(qobj, 0.0, 0.5, 32, 4);

    glEndList();
}

void GLElbowFlexWidget::headSolid(GLUquadricObj* qobj) {

    gluQuadricDrawStyle(qobj, GLU_FILL);

    glNewList(list+SOLID_DISK, GL_COMPILE);

        glColor3f (0.0, 0.0, 0.0);

        gluDisk(qobj, 0.0, 0.5, 32, 4);

        glColor3f (1.0, 1.0, 1.0);

    glEndList();
}

void GLElbowFlexWidget::initializeGL() {

    glClearColor(0.0, 0.0, 0.0, 0.0);

    list = glGenLists(LIST_LENGTH);

    GLUquadricObj* qobj = gluNewQuadric();

    gluQuadricCallback(qobj, GLU_ERROR, errorCallback);

    headSilhouette(qobj);

    headSolid(qobj);

    gluDeleteQuadric(qobj);
}

void GLElbowFlexWidget::reset() {

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

void GLElbowFlexWidget::setState() {

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_DEPTH_TEST);

    glLineWidth(LINE_WIDTH);
}

void GLElbowFlexWidget::shoulder() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 0.0);
        glVertex3d(0.0, 2.0, 2.0*type.sign);
    glEnd();
}

void GLElbowFlexWidget::neck() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 1.0*type.sign);
        glVertex3d(0.0, 2.5, 1.0*type.sign);
    glEnd();
}

void GLElbowFlexWidget::stillUpperArm() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 2.0*type.sign);
        glVertex3d(0.0, 0.0, 2.0*type.sign);
    glEnd();
}

void GLElbowFlexWidget::body() {

    glLineWidth(1.0);

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 1.0*type.sign);
        glVertex3d(0.0,-0.4, 1.0*type.sign);
    glEnd();

    glLineWidth(LINE_WIDTH);
}

void GLElbowFlexWidget::upperArm() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 2.0, 0.0);
    glEnd();
}

void GLElbowFlexWidget::elbow() {

    glPointSize(8.0);

    glBegin(GL_POINTS);
        glVertex3d(0.0, 0.0, 0.0);
    glEnd();

    glPointSize(1.0);
}

void GLElbowFlexWidget::rotateForeArm() {

    glMultMatrixf(rotmat);
}

void GLElbowFlexWidget::foreArm() {

    glBegin(GL_LINES);
        glVertex3d(1.6, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
    glEnd();
}

void GLElbowFlexWidget::hand() {

    glPolygonMode(GL_FRONT, type.handFront);
    glPolygonMode(GL_BACK,  type.handBack);

    glBegin(GL_POLYGON);
       glVertex3d(1.6, -0.15, 0.0);
       glVertex3d(2.0, -0.15, 0.0);
       glVertex3d(2.0,  0.15, 0.0);
       glVertex3d(1.6,  0.15, 0.0);
    glEnd();

    glBegin(GL_LINES);
        glVertex3d(1.7, 0.15, 0.0);
        glVertex3d(1.7, 0.3, 0.0);
    glEnd();

}

void GLElbowFlexWidget::drawIrrelevantParts() {

    glLineWidth(1.0);

    shoulder();

    neck();

    stillUpperArm();

    glLineWidth(LINE_WIDTH);
}

void GLElbowFlexWidget::drawMovingArm() {

    upperArm();

    elbow();

    rotateForeArm();

    foreArm();

    hand();
}

void GLElbowFlexWidget::drawLinearParts() {

    drawIrrelevantParts();

    drawMovingArm();
}

void GLElbowFlexWidget::setCameraPosition() {

    glTranslated(0.0, 0.0, -5.0);
}

void GLElbowFlexWidget::sideHead() {

    glPushMatrix();

        glTranslated(0.0, 3.0, 0.0);

        glPointSize(8.0);

        glBegin(GL_POINTS);
            glVertex3d(0.4, 0.1, 0.0);
        glEnd();

        glPointSize(1.0);

        glCallList(list+SILHOUETTE);


    glPopMatrix();
}

void GLElbowFlexWidget::sideView() {

    glPushMatrix();

        sideHead();

        drawLinearParts();
    glPopMatrix();
}

void GLElbowFlexWidget::writeData() {

    glPushMatrix();

    //(x, y, z): " << rotmat[M11] << ", " << rotmat[M21] << ", " << rotmat[M31];
    // Flex: -90...270; Sup: -180...180; Dev: -90...90

    renderText(-1.0, 3.65, 0.0, data->flex(position).c_str());

    renderText( 2.0, 3.65, 0.0, data->time(position).c_str());

    renderText( 5.0, 3.65, 0.0, data->sup(position).c_str());

    renderText(-1.0, -2.85, 0.0, data->dev(position).c_str());

    renderText(2.5, -2.5, 0.0, "           begin / end / min / max / range  deg");

    renderText(2.5, -3.0, 0.0, data->flex_info());

    renderText(2.5, -3.5, 0.0, data->ext_info());

    renderText(2.5, -4.0, 0.0, data->sup_info());

    renderText(2.5, -4.5, 0.0, data->pron_info());

    renderText(2.5, -5.0, 0.0, data->lat_info());

    renderText(2.5, -5.5, 0.0, data->med_info());

    glPopMatrix();
}

void GLElbowFlexWidget::planHead() {

    glPushMatrix();

        glTranslated(0.0, 1.0*(-type.sign), 2.5);

        glPointSize(8.0);

        glBegin(GL_POINTS);
            glVertex3d(0.4, 0.1, 0.0);
            glVertex3d(0.4,-0.1, 0.0);
        glEnd();

        glPointSize(1.0);

        glCallList(list+SILHOUETTE);

        glTranslated(0.0, 0.0,-0.1);

        glCallList(list+SOLID_DISK);

    glPopMatrix();
}

void GLElbowFlexWidget::planView() {

    glPushMatrix();

        glTranslated(0.0, type.planShift, 0.0);

        planHead();

        glRotated( 90.0, 1.0, 0.0, 0.0);

        drawLinearParts();
    glPopMatrix();
}

void GLElbowFlexWidget::frontHead() {

    glPushMatrix();

        glTranslated(1.0*(-type.sign), 3.0, 0.0);

        glPointSize(8.0);

        glBegin(GL_POINTS);
            glVertex3d(-0.1, 0.1, 0.0);
            glVertex3d( 0.1, 0.1, 0.0);
        glEnd();

        glPointSize(1.0);

        glCallList(list+SILHOUETTE);

    glPopMatrix();
}

void GLElbowFlexWidget::frontView() {

    glPushMatrix();

        glTranslated(type.frontShift, 0.0, 0.0);

        frontHead();

        glRotated(-90.0, 0.0, 1.0, 0.0);

        body();

        drawLinearParts();
    glPopMatrix();
}

void GLElbowFlexWidget::paintGL() {

    reset();

    setCameraPosition();

    setState();

    sideView();

    writeData();

    planView();

    frontView();
}

void GLElbowFlexWidget::resizeGL(int width, int height) {

    const double left  = -2.5;
    const double right =  7.5;
    const double bottom = -7.5;
    const double up     =  4;

    const double w = right-left;
    const double h = up-bottom;

    const double unit = qMin(width/w, height/h);

    glViewport((width-w*unit)/2, (height-h*unit)/2, w*unit, h*unit);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glOrtho(left, right, bottom, up, 2.0, 8.0);

    glMatrixMode(GL_MODELVIEW);
}

void GLElbowFlexWidget::mousePressEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

int GLElbowFlexWidget::numberOfSamples() const {

    return size;
}

void GLElbowFlexWidget::setFrame(int pos) {

    if (pos<0 || pos>=size) {
        throw std::out_of_range("Index is out of range in GLElbowFlexWidget::set_position()");
    }

    position = pos;

    rotate();
}
