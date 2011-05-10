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

#ifndef RECWINDOW_HPP
#define RECWINDOW_HPP

#include <map>
#include <vector>
#include <QWidget>
#include "MatrixVector.hpp"

class QPushButton;
class ArmWidget;

class RecWindow : public QWidget
{
    Q_OBJECT

public:

    static RecWindow* right();

    static RecWindow* left();

    // TODO Display Rec ID, if any, on the title bar
    // TODO Should reference heading and matrices be cleared on show/close?
    // TODO Enable/disable buttons on close?

public slots:

    void updateMatrix(int mote, const gyro::matrix3 rotMat);

protected:

    void keyPressEvent(QKeyEvent * event);

private slots:

    void setReferenceClicked();
    void captureClicked();
    void finishedClicked();
    void nextFrameClicked();
    void dropFrameClicked();
    void saveClicked();
    void clearClicked();

private:

    Q_DISABLE_COPY(RecWindow)

    RecWindow(ArmWidget* w);

    void init();
    void createButtons();
    void setupLayout();
    void setupConnections();

    void setCapturingState();
    void setEditingState();

    void displayCurrentFrame();

    bool areYouSure(const char* text);

    QPushButton* setReferenceButton;
    QPushButton* captureButton;
    QPushButton* finishedButton;

    QPushButton* nextFrameButton;
    QPushButton* dropFrameButton;
    QPushButton* saveButton;
    QPushButton* clearButton;

    ArmWidget* widget;

    typedef std::map<int,gyro::matrix3> Map;
    Map matrices;
    std::vector<Map> frames;
    size_t frameIndex;
};

#endif // RECWINDOW_HPP
