#include "MoteData.h"

#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>
#include <QtDebug>
#include "constants.h"


Sample::Sample()
{
    moteID = 0;
    mote_time = 0;
    counter = 0;
    unix_time = 0;
    xAccel = 0;
    yAccel = 0;
    zAccel = 0;
    xGyro = 0;
    yGyro = 0;
    zGyro = 0;
    voltage = 0;
    temp = 0;
}

MoteData::MoteData()
{
    //samples.reserve(RESERVED_SAMPLES);
}

MoteData::~MoteData()
{
    //samples.clear();
}

void MoteData::setParam(Params param, int value)
{
    if(param == MOTEID){
        moteID = value;
    } else if(param == REBOOTID){
        rebootID = value;
    }
}

void MoteData::setParam(Params param, double value)
{
    if(param == LENGTH){
        length = value;
    } else if(param == SKEW_1){
        skew_1 = value;
    } else if(param == OFFSET){
        offset = value;
    }
}

void MoteData::setBootUnixTime(uint value)
{
    boot_unix_time = value;
}
