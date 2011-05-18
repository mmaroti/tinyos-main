/* Copyright (c) 2011, University of Szeged
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

#include <QObject>
#include <QtGlobal>
#include "Globals.hpp"
#include "AccelMagSample.hpp"
#include "AccelMagMsgReceiver.hpp"
#include "EllipsoidCalibration.hpp"
#include "RecWindow.hpp"
#include "RecordHandler.hpp"
#include "SQLDialog.hpp"
#include "ArmTab.hpp"

// TODO Move extern DataRecorder and rootDirPath here!
// TODO Return bool from connect/disconnect or write an error message to the console

namespace globals {

EllipsoidCalibration* ellipsoid(0);

AccelMagMsgReceiver* accelMagMsgReceiver(0);

void set_Ellipsoid(EllipsoidCalibration* e) {

    ellipsoid = e;
}

void set_AccelMagMsgReceiver(AccelMagMsgReceiver* a) {

    accelMagMsgReceiver = a;
}

void connect_RecWindow_AccelMagMsgReceiver(RecWindow* recWindow) {

    if (accelMagMsgReceiver==0 || recWindow==0) {

        return;
    }

    QObject::connect(accelMagMsgReceiver, SIGNAL(newSample(AccelMagSample)),
                     recWindow,           SLOT(   updateMatrix(AccelMagSample)),
                     Qt::UniqueConnection);
}

void disconnect_RecWindow_AccelMagMsgReceiver(RecWindow* recWindow) {

    if (accelMagMsgReceiver==0 || recWindow==0) {

        return;
    }

    QObject::disconnect(accelMagMsgReceiver, SIGNAL(newSample(AccelMagSample)),
                        recWindow,           SLOT(   updateMatrix(AccelMagSample)));
}

bool connect_ArmTab_AccelMagMsgReceiver(ArmTab* armTab) {

    if (accelMagMsgReceiver==0) {

        return false;
    }

    return QObject::connect(accelMagMsgReceiver, SIGNAL(newSample(AccelMagSample)),
                            armTab,              SLOT(onNewSample(AccelMagSample)),
                            Qt::UniqueConnection);

}

void disconnect_ArmTab_AccelMagMsgReceiver(ArmTab* armTab) {

    QObject::disconnect(accelMagMsgReceiver, SIGNAL(newSample(AccelMagSample)),
                        armTab,              SLOT(onNewSample(AccelMagSample)));
}

void connect_Ellipsoid_AccelMagMsgReceiver() {

    if (ellipsoid==0 || accelMagMsgReceiver==0) {

        return;
    }

    QObject::connect(accelMagMsgReceiver, SIGNAL(newSample(const AccelMagSample )),
                     ellipsoid,           SLOT(onNewSampleReceived(const AccelMagSample )),
                     Qt::UniqueConnection);

}

void disconnect_Ellipsoid_AccelMagMsgReceiver() {

    if (ellipsoid==0 || accelMagMsgReceiver==0) {

        return;
    }

    QObject::disconnect(accelMagMsgReceiver, SIGNAL(newSample(const AccelMagSample )),
                        ellipsoid,           SLOT(onNewSampleReceived(const AccelMagSample )));

}

RecordHandler* recordHandler(0);

void createRecordSelector() {

    if (recordHandler) {

        return;
    }

    recordHandler = new RecordHandler;

    recordHandler->setWindowTitle("Select a record");

    recordHandler->resize(950, 850);
}

RecordHandler* recordSelector() {

    Q_ASSERT(recordHandler);

    return recordHandler;
}


SQLDialog* sqlDialog(0);

void createPersonSelector() {

    if (sqlDialog) {

        return;
    }

    sqlDialog = new SQLDialog;

    sqlDialog->setWindowTitle("Add or select a person");
}

SQLDialog* personSelector() {

    Q_ASSERT(sqlDialog);

    return sqlDialog;
}

}

