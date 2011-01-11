/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

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
