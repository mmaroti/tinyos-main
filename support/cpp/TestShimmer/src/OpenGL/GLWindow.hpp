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

#ifndef GLWINDOW_HPP
#define GLWINDOW_HPP

#include <QWidget>

class GLRightElbowFlex;
class QPushButton;
class QSlider;
class QTimer;

class GLWindow : public QWidget
{
    Q_OBJECT

public:

    GLWindow();

    GLWindow(double* rotmat, int size);

signals:

    void closed();

protected:

    void keyPressEvent(QKeyEvent * event);

private slots:

    void closeEvent(QCloseEvent *);

    void nextFrame();
    void toggleAnimationState();
    void setFrame(int pos);

private:

    Q_DISABLE_COPY(GLWindow)


    void createGLWidget();
    void createGLWidget(double* rotmat, int size);
    void init();
    void createButton();
    void createSlider();
    void createTimer();
    void setupLayout();
    void setupConnections();

    void timerStart();
    void timerStop();

    const int ANIMATION_STEP_MS;

    GLRightElbowFlex* widget;
    QPushButton* playButton;
    QSlider* slider;
    QTimer*   timer;
};

#endif // GLWINDOW_HPP
