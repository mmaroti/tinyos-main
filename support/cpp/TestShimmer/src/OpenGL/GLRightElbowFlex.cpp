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
#include "GLRightElbowFlex.hpp"
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

GLRightElbowFlex::GLRightElbowFlex(AnimationElbowFlexSign right_or_left,
                                   QWidget *parent,
                                   QGLWidget *shareWidget)
: QGLWidget(parent, shareWidget),
  type(right_or_left)
{
    for (int i=0;i<16; ++i)
        rotmat[i] = (GLfloat) 0.0;

    rotmat[M11] = rotmat[M22] = rotmat[M33] = rotmat[M44] = (GLfloat) 1.0;

    position = size = 0;

    data = 0;
}

void GLRightElbowFlex::setData(const char *filename) {

    delete data;

    data = new DataHolder(ElbowFlexSign::right());

    data->grab_content(filename);

    size = data->number_of_samples();

    position = 0;
}

void GLRightElbowFlex::setData(double* rotmat, int length) {

    delete data; // FIXME Duplication

    data = new DataHolder(ElbowFlexSign::right());

    data->set_content(rotmat, length);

    size = data->number_of_samples();

    position = 0;
}

GLRightElbowFlex::~GLRightElbowFlex() {

    delete data;
}

QSize GLRightElbowFlex::minimumSizeHint() const {

    return QSize(300, 300);
}

QSize GLRightElbowFlex::sizeHint() const {

    return QSize(750, 750);
}

// TODO Threading?
void GLRightElbowFlex::rotate() {

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

void GLRightElbowFlex::headSilhouette(GLUquadricObj* qobj) {

    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);

    gluQuadricNormals(qobj, GLU_NONE);

    glNewList(list+SILHOUETTE, GL_COMPILE);

        gluDisk(qobj, 0.0, 0.5, 32, 4);

    glEndList();
}

void GLRightElbowFlex::headSolid(GLUquadricObj* qobj) {

    gluQuadricDrawStyle(qobj, GLU_FILL);

    glNewList(list+SOLID_DISK, GL_COMPILE);

        glColor3f (0.0, 0.0, 0.0);

        gluDisk(qobj, 0.0, 0.5, 32, 4);

        glColor3f (1.0, 1.0, 1.0);

    glEndList();
}

void GLRightElbowFlex::initializeGL() {

    glClearColor(0.0, 0.0, 0.0, 0.0);

    list = glGenLists(LIST_LENGTH);

    GLUquadricObj* qobj = gluNewQuadric();

    gluQuadricCallback(qobj, GLU_ERROR, errorCallback);

    headSilhouette(qobj);

    headSolid(qobj);

    gluDeleteQuadric(qobj);
}

void GLRightElbowFlex::reset() {

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

void GLRightElbowFlex::setState() {

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_DEPTH_TEST);

    glLineWidth(LINE_WIDTH);
}

void GLRightElbowFlex::shoulder() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 0.0);
        glVertex3d(0.0, 2.0, 2.0*type.z_sign);
    glEnd();
}

void GLRightElbowFlex::neck() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 1.0*type.z_sign);
        glVertex3d(0.0, 2.5, 1.0*type.z_sign);
    glEnd();
}

void GLRightElbowFlex::leftUpperArm() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 2.0*type.z_sign);
        glVertex3d(0.0, 0.0, 2.0*type.z_sign);
    glEnd();
}

void GLRightElbowFlex::body() {

    glLineWidth(1.0);

    glBegin(GL_LINES);
        glVertex3d(0.0, 2.0, 1.0*type.z_sign);
        glVertex3d(0.0,-0.4, 1.0*type.z_sign);
    glEnd();

    glLineWidth(LINE_WIDTH);
}

void GLRightElbowFlex::upperArm() {

    glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 2.0, 0.0);
    glEnd();
}

void GLRightElbowFlex::elbow() {

    glPointSize(8.0);

    glBegin(GL_POINTS);
        glVertex3d(0.0, 0.0, 0.0);
    glEnd();

    glPointSize(1.0);
}

void GLRightElbowFlex::rotateForeArm() {

    glMultMatrixf(rotmat);
}

void GLRightElbowFlex::foreArm() {

    glBegin(GL_LINES);
        glVertex3d(1.6, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
    glEnd();
}

void GLRightElbowFlex::hand() {

    glPolygonMode(GL_FRONT, type.hand_front);
    glPolygonMode(GL_BACK,  type.hand_back);

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

void GLRightElbowFlex::drawIrrelevantParts() {

    glLineWidth(1.0);

    shoulder();

    neck();

    leftUpperArm();

    glLineWidth(LINE_WIDTH);
}

void GLRightElbowFlex::drawRightArm() {

    upperArm();

    elbow();

    rotateForeArm();

    foreArm();

    hand();
}

void GLRightElbowFlex::drawLinearParts() {

    drawIrrelevantParts();

    drawRightArm();
}

void GLRightElbowFlex::setCameraPosition() {

    glTranslated(0.0, 0.0, -5.0);
}

void GLRightElbowFlex::sideHead() {

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

void GLRightElbowFlex::sideView() {

    glPushMatrix();

        sideHead();

        drawLinearParts();
    glPopMatrix();
}

void GLRightElbowFlex::writeData() {

    glPushMatrix();

    //(x, y, z): " << rotmat[M11] << ", " << rotmat[M21] << ", " << rotmat[M31];
    // Flex: -90...270; Sup: -180...180; Dev: -90...90

    renderText(-1.0, 3.65, 0.0, data->flex(position).c_str());

    renderText( 2.0, 3.65, 0.0, data->time(position).c_str());

    renderText( 5.0, 3.65, 0.0, data->sup(position).c_str());

    renderText(-1.0, -2.85, 0.0, data->dev(position).c_str());

    renderText(3.0, -2.5, 0.0, "           begin / min / max / range  deg");

    renderText(3.0, -3.0, 0.0, data->flex_info());

    renderText(3.0, -3.5, 0.0, data->sup_info());

    renderText(3.0, -4.0, 0.0, data->pron_info());

    renderText(3.0, -4.5, 0.0, data->med_info());

    renderText(3.0, -5.0, 0.0, data->lat_info());

    glPopMatrix();
}

void GLRightElbowFlex::planHead() {

    glPushMatrix();

        glTranslated(0.0, 1.0, 2.5);

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

void GLRightElbowFlex::planView() {

    glPushMatrix();

        glTranslated(0.0, -5.0, 0.0);

        planHead();

        glRotated( 90.0, 1.0, 0.0, 0.0);

        drawLinearParts();
    glPopMatrix();
}

void GLRightElbowFlex::frontHead() {

    glPushMatrix();

        glTranslated(1.0, 3.0, 0.0);

        glPointSize(8.0);

        glBegin(GL_POINTS);
            glVertex3d(-0.1, 0.1, 0.0);
            glVertex3d( 0.1, 0.1, 0.0);
        glEnd();

        glPointSize(1.0);

        glCallList(list+SILHOUETTE);

    glPopMatrix();
}

void GLRightElbowFlex::frontView() {

    glPushMatrix();

        glTranslated( 5.0, 0.0, 0.0);

        frontHead();

        glRotated(-90.0, 0.0, 1.0, 0.0);

        body();

        drawLinearParts();
    glPopMatrix();
}

void GLRightElbowFlex::paintGL() {

    reset();

    setCameraPosition();

    setState();

    sideView();

    writeData();

    planView();

    frontView();
}

void GLRightElbowFlex::resizeGL(int width, int height) {

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

void GLRightElbowFlex::mousePressEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

int GLRightElbowFlex::numberOfSamples() const {

    return size;
}

void GLRightElbowFlex::setFrame(int pos) {

    if (pos<0 || pos>=size) {
        throw std::out_of_range("Index is out of range in GLRightElbowFlex::set_position()");
    }

    position = pos;

    rotate();
}

