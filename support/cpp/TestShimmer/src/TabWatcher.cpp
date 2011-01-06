/* Copyright (c) 2010, University of Szeged
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
*      Author: Ali Baharev
*/

#include <QTabWidget>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include "TabWatcher.hpp"

TabWatcher::TabWatcher() : tabWidget(0) {

}

void TabWatcher::registerTabWidget(const QTabWidget* widget) {

    Q_ASSERT(tabWidget==0);

    tabWidget = widget;

    selected(tabWidget->currentIndex());

    QObject::connect(tabWidget, SIGNAL(currentChanged(int)),
                     this,      SLOT(tabChanged(int)));
}

void TabWatcher::tabChanged(int tab) {

    selected(tab);
}

void TabWatcher::selected(int tab) {

    Q_ASSERT(tab!=-1);

    QString label = tabWidget->tabText(tab);

    QString dir = selectDir(label);

    bool success = QDir::setCurrent(dir);

    if (!success) {

        exitFailure(dir);
    }

    emit_signal(label);

    qDebug() << "Working directory is " << QDir::currentPath();
}

const QString TabWatcher::selectDir(const QString& label) const {

    QString dir("../tmp");

    if (label=="SD Card") {

        dir = "../rec";
    }

    return dir;
}

void TabWatcher::emit_signal(const QString& label) {

    if (label=="SD Card") {

        emit SDCardTabGainedFocus();
    }
}

void TabWatcher::exitFailure(const QString& dir) const {

    QString msg("Error: failed to set directory ");

    msg.append(dir);
    msg.append("\nRunning from:\n");
    msg.append(QDir::currentPath());

    QMessageBox mbox;

    mbox.setText(msg);
    mbox.exec();

    exit(EXIT_FAILURE);
}
