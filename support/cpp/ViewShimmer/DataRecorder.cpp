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


#include <assert.h>
#include "DataRecorder.h"
#include "Application.h"
#include <QtDebug>
#include <qfile.h>
#include <QStringList>
#include <QMessageBox>
#include <math.h>
#include "constants.h"


namespace {

    const int RESERVED_SAMPLES = 100000;
}

DataRecorder::DataRecorder(Application &app) :
        application(app)
{

    samples.reserve(RESERVED_SAMPLES);

}

DataRecorder::~DataRecorder() {
}



void DataRecorder::onReceiveMessage(const ActiveMessage & msg)
{
    // first type of messages
    if( msg.type == 0x37 && msg.payload.size() == 100 )
    {

            for(int start = 0; start < 100; start += 20)
            {
                    Sample sample;

                    sample.time = msg.getInt(start);
                    sample.xAccel = msg.getShort(start + 4);
                    sample.yAccel = msg.getShort(start + 6);
                    sample.zAccel = msg.getShort(start + 8);
                    sample.xGyro = msg.getShort(start + 10);
                    sample.yGyro = msg.getShort(start + 12);
                    sample.zGyro = msg.getShort(start + 14);
                    sample.voltage = msg.getShort(start + 16);
                    sample.temp = msg.getShort(start + 18);

    //			qDebug() << "sample " + sample.toString();

                    /*if(samples.size() > 2){
                        int lag = getLag(sample.time);
                        if(lag > MAX_DUMMY) lag = MAX_DUMMY;
                        for(int i=lag; i > 0; i--){
                            Sample dummy;
                            samples.append(dummy);
                        }
                    }*/
                    samples.append(sample);
            }
            emit sampleAdded();
    }
}

Sample::Sample()
{
        time = 0;
        xAccel = 0;
        yAccel = 0;
        zAccel = 0;
        xGyro = 0;
        yGyro = 0;
        zGyro = 0;
        voltage = 0;
        temp = 0;
}
