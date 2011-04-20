/** Copyright (c) 2010, 2011 University of Szeged
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

#include "ActiveMessage.hpp"

unsigned char ActiveMessage::getByte(int index) const
{
        return (unsigned char)payload.at(index);
}

unsigned short ActiveMessage::getShort(int index) const
{
        return (payload.at(index) & 0xFF) + ((payload.at(index + 1) & 0xFF) << 8);
}

unsigned int ActiveMessage::getID() const
{
        return source;
}

unsigned int ActiveMessage::getInt(int index) const
{
        unsigned int a, b, c, d;

        a = payload.at(index);
        b = payload.at(index + 1);
        c = payload.at(index + 2);
        d = payload.at(index + 3);

        a &= 0xFF;
        b &= 0xFF;
        c &= 0xFF;
        d &= 0xFF;

        return (d << 24) + (c << 16) + (b << 8) + a;
}

QString ActiveMessage::toString() const
{
        QString s = "(dst:" + QString::number(dest)
                + " src:" + QString::number(source)
                + " grp:" + QString::number(group)
                + " typ:" + QString::number(type)
                + " len:" + QString::number(payload.size())
                + " bytes:";

        for(int i = 0; i < payload.size(); ++i)
        {
                s += " 0x";

                QString t = QString::number(payload.at(i) & 0xFF, 16);
                if( t.length() == 1 )
                        s += '0';

                s += t;
        }

        return s + ')';
}
