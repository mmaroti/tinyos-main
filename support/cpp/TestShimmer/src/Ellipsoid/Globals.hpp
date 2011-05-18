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

#include "MatrixVector.hpp"

class ArmTab;
class EllipsoidCalibration;
class AccelMagMsgReceiver;
class RecWindow;
class RecordHandler;
class SQLDialog;

// FIXME A rather messy workaround, the singleton pattern should be used

namespace globals {

void connect_Ellipsoid_AccelMagMsgReceiver();

void disconnect_Ellipsoid_AccelMagMsgReceiver();

void set_Ellipsoid(EllipsoidCalibration* ellipsoid);

void set_AccelMagMsgReceiver(AccelMagMsgReceiver* accelMagMsgReceiver);

bool connect_ArmTab_AccelMagMsgReceiver(ArmTab* armTab);

void disconnect_ArmTab_AccelMagMsgReceiver(ArmTab* armTab);

void connect_RecWindow_AccelMagMsgReceiver(RecWindow* recWindow);

void disconnect_RecWindow_AccelMagMsgReceiver(RecWindow* recWindow);

void createRecordSelector();

RecordHandler* recordSelector();

void createPersonSelector();

SQLDialog* personSelector();

bool resetScaleOffset(int mote);

bool updateScaleOffset(int mote, const gyro::vector3& accScl,  const gyro::vector3& accOff,
                                 const gyro::vector3& magnScl, const gyro::vector3& magnOff);

}
