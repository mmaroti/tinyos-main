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
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include <QtGui>
#include <QtOpenGL>

#include "GLWidget.h"

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
}

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
    clearColor = Qt::black;

    for (int i=0;i<16; ++i)
        rotmat[i] = (GLfloat) 0.0;

    rotmat[M11] = rotmat[M22] = rotmat[M33] = rotmat[M44] = (GLfloat) 1.0;

    rotmat[M34] = (GLfloat) -10.0;
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void GLWidget::rotate(const double mat[9])
{
    rotmat[M11] = (GLfloat) mat[R11];
    rotmat[M21] = (GLfloat) mat[R21];
    rotmat[M31] = (GLfloat) mat[R31];

    rotmat[M12] = (GLfloat) mat[R12];
    rotmat[M22] = (GLfloat) mat[R22];
    rotmat[M32] = (GLfloat) mat[R32];

    rotmat[M13] = (GLfloat) mat[R13];
    rotmat[M23] = (GLfloat) mat[R23];
    rotmat[M33] = (GLfloat) mat[R33];

    updateGL();
}

void GLWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    updateGL();
}

void GLWidget::initializeGL()
{
    makeObject();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#ifndef QT_OPENGL_ES_2
    glEnable(GL_TEXTURE_2D);
#endif

}

void GLWidget::paintGL()
{
    qglClearColor(clearColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadMatrixf(rotmat);

    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for (int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

#if !defined(QT_OPENGL_ES_2)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifndef QT_OPENGL_ES
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
#else
    glOrthof(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
#endif
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

void GLWidget::makeObject()
{
    static  float coords[6][4][3] = {
        { { +0.8, -1.5, -2.5 }, { -0.8, -1.5, -2.5 }, { -0.8, +1.5, -2.5 }, { +0.8, +1.5, -2.5 } },
        { { +0.8, +1.5, -2.5 }, { -0.8, +1.5, -2.5 }, { -0.8, +1.5, +2.5 }, { +0.8, +1.5, +2.5 } },
        { { +0.8, -1.5, +2.5 }, { +0.8, -1.5, -2.5 }, { +0.8, +1.5, -2.5 }, { +0.8, +1.5, +2.5 } },
        { { -0.8, -1.5, -2.5 }, { -0.8, -1.5, +2.5 }, { -0.8, +1.5, +2.5 }, { -0.8, +1.5, -2.5 } },
        { { +0.8, -1.5, +2.5 }, { -0.8, -1.5, +2.5 }, { -0.8, -1.5, -2.5 }, { +0.8, -1.5, -2.5 } },
        { { -0.8, -1.5, +2.5 }, { +0.8, -1.5, +2.5 }, { +0.8, +1.5, +2.5 }, { -0.8, +1.5, +2.5 } }
    };

    for (int j=0; j < 6; ++j) {
        textures[j] = bindTexture
            (QPixmap(QString(":/images/side%1.jpg").arg(j + 1)), GL_TEXTURE_2D);
    }

    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            texCoords.append
                (QVector2D(j == 0 || j == 3, j == 0 || j == 1));
            vertices.append
                (QVector3D(0.2 * coords[i][j][0], 0.2 * coords[i][j][1],
                           0.2 * coords[i][j][2]));
        }
    }
}
