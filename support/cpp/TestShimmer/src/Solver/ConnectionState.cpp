/** Copyright (c) 2011, University of Szeged
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

#include <QTimer>
#include "ConnectionState.hpp"

ConnectionState::ConnectionState() :
        connected(false), received(false), state(RED), timer(new QTimer)
{

}

ConnectionState::~ConnectionState() {

    delete timer;
}

bool ConnectionState::isConnected() const {

    return connected;
}

void ConnectionState::timerFired() {

    if (!connected) {

        state = RED;
    }
    else if (!received) {

        state = YELLOW;
    }
    else {

        state = GREEN;
    }

    received = false;

    emit color(state);
}

void ConnectionState::change(const QString& , int connected) {

    if (connected == 1) {

        connectedToPort();
    }
    else if (connected == 0) {

        disconnected();
    }
}

void ConnectionState::connectedToPort() {

    connected = true;

    received = false;

    timer->start(1000);
}

void ConnectionState::disconnected() {

    timer->stop();

    connected = false;

    received = false;

    emit color(RED);
}

void ConnectionState::msgReceived(const ActiveMessage& ) {

    received = true;
}
